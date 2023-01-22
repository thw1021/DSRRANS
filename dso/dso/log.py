from time import localtime, strftime
import os
# from dso.config import load_config

# Set to false if you do not want to save log outputs to file
logfile = True

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class uid:
    moment = strftime("%Y-%m-%d-%H%M%S",localtime())

class Log():
    '''
    Class for logging outputs with colors and timestamps
    '''
    # def __init__(self, config):
    #     assert config is not None, "Need configuration file to specify the task name for log"
    #     config = load_config(config)
    #     assert config["experiment"]["logdir"] is not None, "Please specify the logdir"
    #     task_name = config["task"]["dataset"]
    #     timestamp = uid.moment
    #     self.log_path = os.path.join(config["experiment"]["logdir"], '_'.join([task_name, timestamp]))
    #     self.log_file = os.path.join(self.log_path, "output_{}.dat".format(uid.moment))

    def __init__(self):
        if not os.path.exists("log"):
            os.makedirs("log")

    def logging(self, str0):
        base_str = strftime("[%m%d-%H:%M:%S]", localtime())
        output_type = "[Output]: "
        print(base_str+output_type+str0)
        if(logfile):
            with open('log/output'+'_'+uid.moment+'.dat', 'a') as f:
                f.write(base_str+output_type+str0+'\n')

    def info(self, str0):
        base_str = strftime("[%m%d-%H:%M:%S]", localtime())
        output_type = "[Info]: "
        print(bcolors.OKBLUE+base_str+output_type+str0+bcolors.ENDC)
        if(logfile):
            with open('log/output'+'_'+uid.moment+'.dat', 'a') as f:
                f.write(base_str+output_type+str0+'\n')

    def success(self, str0):
        base_str = strftime("[%m%d-%H:%M:%S]", localtime())
        output_type = "[Success]: "
        print(bcolors.OKGREEN+base_str+output_type+str0+bcolors.ENDC)
        if(logfile):
            with open('log/output'+'_'+uid.moment+'.dat', 'a') as f:
                f.write(base_str+output_type+str0+'\n')

    def warning(self, str0):
        base_str = strftime("[%m%d-%H:%M:%S]", localtime())
        output_type = "[Warning]: "
        print(bcolors.WARNING+base_str+output_type+str0+bcolors.ENDC)
        if(logfile):
            with open('log/output'+'_'+uid.moment+'.dat', 'a') as f:
                f.write(base_str+output_type+str0+'\n')

    def error(self, str0):
        base_str = strftime("[%m%d-%H:%M:%S]", localtime())
        output_type = "[Error]: "
        print(bcolors.FAIL+base_str+output_type+str0+bcolors.ENDC)
        if(logfile):
            with open('log/output'+'_'+uid.moment+'.dat', 'a') as f:
                f.write(base_str+output_type+str0+'\n')