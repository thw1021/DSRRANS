import numpy as np

from dso.program import from_tokens
from dso.utils import weighted_quantile
from dso.log import Log


def quantile_variance(memory_queue, controller, batch_size, epsilon, step,
                      n_experiments=1000, estimate_bias=True,
                      n_samples_bias=1e6):

    lg = Log()

    lg.logging("Running quantile variance/bias experiments...")
    empirical_quantiles = []
    memory_augmented_quantiles = []

    if len(memory_queue) < memory_queue.capacity:
        lg.warning("Memory queue not yet at capacity.")

    memory_r = memory_queue.get_rewards()
    memory_w = memory_queue.compute_probs()
    for exp in range(n_experiments):
        actions, obs, priors = controller.sample(batch_size)
        programs = [from_tokens(a) for a in actions]
        r = np.array([p.r for p in programs])
        quantile = np.quantile(r, 1 - epsilon, interpolation="higher")
        empirical_quantiles.append(quantile)
        unique_programs = [p for p in programs if p.str not in memory_queue.unique_items]
        N = len(unique_programs)
        sample_r = [p.r for p in unique_programs]
        combined_r = np.concatenate([memory_r, sample_r])
        if N == 0:
            lg.warning("Found no unique samples in batch!")
            combined_w = memory_w / memory_w.sum() # Renormalize
        else:
            sample_w = np.repeat((1 - memory_w.sum()) / N, N)
            combined_w = np.concatenate([memory_w, sample_w])

        # Compute the weighted quantile
        quantile = weighted_quantile(values=combined_r, weights=combined_w, q=1 - epsilon)
        memory_augmented_quantiles.append(quantile)

    empirical_quantiles = np.array(empirical_quantiles)
    memory_augmented_quantiles = np.array(memory_augmented_quantiles)
    lg.logging("Train step:", step)
    lg.logging("Memory weight:", memory_w.sum())
    lg.logging("Mean(empirical quantile):", np.mean(empirical_quantiles))
    lg.logging("Var(empirical quantile):", np.var(empirical_quantiles))
    lg.logging("Mean(Memory augmented quantile):", np.mean(memory_augmented_quantiles))
    lg.logging("Var(Memory augmented quantile):", np.var(memory_augmented_quantiles))
    if estimate_bias:
        actions, obs, priors = controller.sample(int(n_samples_bias))
        programs = [from_tokens(a) for a in actions]
        r = np.array([p.r for p in programs])
        true_quantile = np.quantile(r, 1 - epsilon, interpolation="higher")
        lg.logging("'True' empirical quantile:", true_quantile)
        lg.logging("Empirical quantile bias:", np.mean(np.abs(empirical_quantiles - true_quantile)))
        lg.logging("Memory-augmented quantile bias:", np.mean(np.abs(memory_augmented_quantiles - true_quantile)))
    exit()
