/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2013-2018 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "dsoRStress.H"
#include "addToRunTimeSelectionTable.H"
#include <vector>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace RASModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(dsoRStress, 0);
addToRunTimeSelectionTable(RASModel, dsoRStress, dictionary);

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void dsoRStress::correctNut()
{
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

dsoRStress::dsoRStress
(
    const geometricOneField& alpha,
    const geometricOneField& rho,
    const volVectorField& U,
    const surfaceScalarField& alphaRhoPhi,
    const surfaceScalarField& phi,
    const transportModel& transport,
    const word& propertiesName,
    const word& type
)
:
    eddyViscosity<incompressible::RASModel>
    (
        type,
        alpha,
        rho,
        U,
        alphaRhoPhi,
        phi,
        transport,
        propertiesName
    ),

    alpha1_
    (
        dimensioned<scalar>::lookupOrAddToDict
        (
            "alpha1",
            this->coeffDict_,
            1.0
        )
    ),

    invScale_
    (
        Switch::lookupOrAddToDict
        (
            "invScale",
            this->coeffDict_,
            true
        )
    ),

    nut_
    (
        IOobject
        (
            "nut",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    ),

    k_
    (
        IOobject
        (
            "k",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    ),

    epsilon_
    (
        IOobject
        (
            "epsilon",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    ),

    T1_
    (
        IOobject
        (
            "T1",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedTensor("T1", dimless, tensor::zero)
    ),

    T2_
    (
        IOobject
        (
            "T2",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedTensor("T2", dimless, tensor::zero)
    ),

    T3_
    (
        IOobject
        (
            "T3",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedTensor("T3", dimless, tensor::zero)
    ),

    bBot_
    (
        IOobject
        (
            "bBot",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedTensor("bBot", dimless, tensor::zero)
    ),

    aBot_
    (
        IOobject
        (
            "aBot",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedTensor("aBot", dimensionSet(0, 2, -2, 0, 0, 0, 0), tensor::zero)
    ),

    a_
    (
        IOobject
        (
            "a",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_,
        dimensionedTensor("a", dimensionSet(0, 2, -2, 0, 0, 0, 0), tensor::zero)
    )

{
    // bound(k_, this->kMin_);
    // bound(epsilon_, this->epsilonMin_);
    if (type == typeName)
    {
        printCoeffs(type);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool dsoRStress::read()
{
    if (eddyViscosity<incompressible::RASModel>::read())
    {
        return true;
    }
    else
    {
        return false;
    }
}


void dsoRStress::correct()
{
    if (!turbulence_)
    {
        return;
    }

    eddyViscosity<incompressible::RASModel>::correct();
}

Foam::tmp<Foam::fvVectorMatrix>
dsoRStress::divDevRhoReff
(
    volVectorField& U
) const
{
    label timeIndex = this->mesh_.time().timeIndex();
    label startTime = this->runTime_.startTimeIndex();

    size_t sizei = 2;
    size_t sizet = 3;

    if(timeIndex - 1 == startTime){
        Info << "Entered Data-driven turbulence predictor" << endl;
        Info << "Calculating Flow Field Invariants" << endl;

        volScalarField ke = k_ / epsilon_;

        volTensorField UGrad(fvc::grad(U));
        volTensorField R(-ke * skew(UGrad));
        volTensorField S(R);
        volSymmTensorField sym = ke * symm(UGrad);
        S.replace(0, sym.component(symmTensor::XX));
        S.replace(1, sym.component(symmTensor::XY));
        S.replace(2, sym.component(symmTensor::XZ));
        S.replace(3, sym.component(symmTensor::XY));
        S.replace(4, sym.component(symmTensor::YY));
        S.replace(5, sym.component(symmTensor::YZ));
        S.replace(6, sym.component(symmTensor::XZ));
        S.replace(7, sym.component(symmTensor::YZ));
        S.replace(8, sym.component(symmTensor::ZZ));

        //- Get the invariant inputs to the NN
        std::vector<volScalarField*> invar(sizei);
        std::vector<volScalarField*> invar0(sizei);
        invar[0] = new volScalarField(tr(S&S));
        invar[1] = new volScalarField(tr(R&R));
        invar0[0] = new volScalarField(*invar[0]);
        invar0[1] = new volScalarField(*invar[1]);
        if (this->invScale_){
            Info << "Scale input invariants" << endl;
            forAll(this->mesh_.C(), cell){
                double I1 = static_cast<double> ((*invar[0])[cell]);
                double I2 = static_cast<double> ((*invar[1])[cell]);
                (*invar0[0])[cell] = Foam::sign(I1)*(1-std::exp(-abs(I1)))/(1+std::exp(-abs(I1)));
                (*invar0[1])[cell] = Foam::sign(I2)*(1-std::exp(-abs(I2)))/(1+std::exp(-abs(I2)));
            }
        }

        Info << "Calculating Flow Field tensor basis functions" << endl;
        std::vector<volTensorField*> tensorf(sizet);
        tensorf[0] = new volTensorField(S);
        tensorf[1] = new volTensorField((S&R) - (R&S));
        tensorf[2] = new volTensorField((S&S) - 1./3*I*(*invar[0]));

        //- Now get the data-driven prediction
        Info << "Executing forward pass of the symbolic expressions" << endl;
        forAll(this->mesh_.C(), cell){
            #include "turbSymbolicExpression.H"
        }
        this->aBot_ = alpha1_*this->alpha_*this->rho_*this->k_*(this->bBot_);
        Info << "Job Done" << '\n' << endl;
    }
    //- The anisotropic Reynolds stress tensor
    this->a_ = this->aBot_ - this->alpha_*this->rho_*this->nut_*(I & dev(twoSymm(fvc::grad(U))));

    return
    (
        - fvm::laplacian(this->alpha_*this->rho_*(this->nu()+this->nut_), U)
        - fvc::div((this->alpha_*this->rho_*(this->nu()+this->nut_))*dev2(T(fvc::grad(U))))
        + fvc::div(this->aBot_)
    );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace incompressible
} // End namespace Foam

// ************************************************************************* //
