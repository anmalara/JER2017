import sys
import os
import time

sys.path.append("/nfs/dust/cms/user/amalara/WorkingArea/UHH2_94X_v2/CMSSW_9_4_1/src/UHH2/PersonalCode/")
from parallelise import *

def getLabel(sample):
  if sample == "B":
    LABEL_LUMI_INV_FB = "[MC 94X] Run2017B 4.77 fb^{-1}"
  elif sample == "C":
    LABEL_LUMI_INV_FB = "[MC 94X] Run2017C 9.58 fb^{-1}"
  elif sample == "D":
    LABEL_LUMI_INV_FB="[MC 94X] Run2017D 4.22 fb^{-1}"
  elif sample == "E":
    LABEL_LUMI_INV_FB="[MC 94X] Run2017E 9.26 fb^{-1}"
  elif sample == "F":
    LABEL_LUMI_INV_FB="[MC 94X] Run2017F 13.46 fb^{-1}"
  elif sample == "BC":
    LABEL_LUMI_INV_FB="[MC 94X] Run2017BC 14.35 fb^{-1}"
  elif sample == "DE":
    LABEL_LUMI_INV_FB="[MC 94X] Run2017DE 13.48 fb^{-1}"
  elif sample == "DEF":
    LABEL_LUMI_INV_FB="[MC 94X] Run2017DEF 26.95 fb^{-1}"
  elif sample == "BCDEF":
    LABEL_LUMI_INV_FB="[MC 94X] 2017 41.53 fb^{-1}"
  else:
    LABEL_LUMI_INV_FB="[MC 94X] (2017)"
  return LABEL_LUMI_INV_FB


def main_function(gaustails=False, shiftForPLI="central"):
  outdir = out_path+pattern+QCDsample+"/"+run+"/"
  gaustail_num = 0.985
  shiftForPLI_num = 0.0
  ref_shift = 3
  if "barrel_check" in extraText:
    ref_shift = int(extraText[-2])
  if gaustails:
    outdir = out_path+newJECVersion+"/"+newJetLabel+"/gaustails/"+QCDsample+"/"+run+"/"
    gaustail_num = 0.95
  if shiftForPLI=="up":
    outdir = out_path+newJECVersion+"/"+newJetLabel+"/PLI/up/"+QCDsample+"/"+run+"/"
    shiftForPLI_num = 0.25
  if shiftForPLI=="down":
    outdir = out_path+newJECVersion+"/"+newJetLabel+"/PLI/down/"+QCDsample+"/"+run+"/"
    shiftForPLI_num = -0.25
  # print "outdir ", outdir
  if os.path.isdir(outdir):
    for el in sorted(os.listdir(outdir)):
      cmd = "rm -fr %s" % (outdir+el)
      a = os.system(cmd)
  else:
    os.makedirs(outdir)
  cmd = "cp mainRun.cxx %s" % (outdir)
  a = os.system(cmd)
  cmd = "cp functions.C %s" % (outdir)
  a = os.system(cmd)
  cmd = "cp /nfs/dust/cms/user/amalara/WorkingArea/UHH2_94X_v2/CMSSW_9_4_1/src/UHH2/PersonalCode/tdrstyle_all.C %s" % (outdir)
  a = os.system(cmd)
  os.chdir(outdir)
  os.makedirs("pdfy")
  os.makedirs("pdfy/MCTruth")
  os.makedirs("pdfy/SFs")
  os.makedirs("pdfy/NSC_SFs")
  os.makedirs("pdfy/JERs")
  os.makedirs("pdfy/widths")
  os.makedirs("pdfy/maps")
  os.makedirs("ClosureTest")
  os.makedirs("output")
  os.makedirs("output/asymmetries")
  # print pattern+run
  temp_time=time.time()
  # f = open("log.txt",'w')
  MC_type = '\\"MC\\"'
  data_type = '\\"Data\\"'
  trigger_type = '\\"'+study[:-1]+'\\"'
  cmd = '"%smainRun.cxx(false, %s, %s, %s, %s , %s, %s, %s, %s, %s, %s)"' % (outdir, MC_file, Data_file, LABEL_LUMI_INV_FB, MC_type, data_type, trigger_type, '\\"'+outdir+'\\"', gaustail_num, shiftForPLI_num, ref_shift)
  # cmd = 'root -l -b -q "%smainRun.cxx(false, %s, %s, %s, %s , %s, %s, %s, %s, %s, %s)" >> log.txt  ' % (outdir, MC_file, Data_file, LABEL_LUMI_INV_FB, MC_type, data_type, trigger_type, '\\"'+outdir+'\\"', gaustail_num, shiftForPLI_num, ref_shift)
  command = ["root", "-l", "-b", "-q", "plot_SF_systematics.cxx"]
  list_processes.append(command)
  print cmd
  # a = os.system(cmd)
  print ("time needed: "+str((time.time()-temp_time))+" s")
  os.chdir(common_path)




source_path = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_94X_v2/CMSSW_9_4_1/src/UHH2/JER2017/Analysis/hist_preparation/"
common_path = "/nfs/dust/cms/user/amalara/WorkingArea/UHH2_94X_v2/CMSSW_9_4_1/src/UHH2/JER2017/Analysis/JER/wide_eta_binning/"


samples = ["B","C","D","E","F","BC","DE","DEF","BCDEF"]
samples = ["BCDEF"]
# QCDSamples = ["QCDPt","QCDHT"]
QCDSamples = ["QCDPt"]
JetLabels=["AK4CHS"]
JECVersions=["Fall17_17Nov2017_V27"]
dirs = ["", "up", "down"]
# studies = ["StandardPtBins/", "StandardPtBins_L1Seed/"]
studies = ["StandardPtBins_all/"]
# studies = ["StandardPtBins_weightcut/"]
systematics=["", "PU", "JEC", "alpha"]
systematics=[""]

list_processes = []
list_logfiles = []

for extraText in [""]:
  for study in studies:
    out_path  = common_path+"file/"+study+extraText
    for newJECVersion in JECVersions:
      for newJetLabel in JetLabels:
        for syst in systematics:
          for dir in dirs:
            if (syst == "" and dir != "") or (syst == "alpha" and dir != "") or ((syst != "" and syst != "alpha") and dir == ""):
              continue
            pattern = newJECVersion+"/"+newJetLabel+"/"+syst+"/"+dir+"/"
            if syst == "":
              pattern = newJECVersion+"/"+newJetLabel+"/standard/"
            if syst == "alpha":
              pattern = newJECVersion+"/"+newJetLabel+"/"+syst+"/"
            print "pattern ", pattern
            for QCDsample in QCDSamples:
              for sample in samples:
                run = "Run"+sample
                LABEL_LUMI_INV_FB=getLabel(sample)
                LABEL_LUMI_INV_FB = '\\"'+LABEL_LUMI_INV_FB+'\\"'
                # MC_file   = '\\"'+source_path+"MC/wide_eta_bin/file/save_v1/"+study+pattern.replace("/standard","")+QCDsample+extraText+"/histograms_mc_incl_full.root"+'\\"'
                # Data_file = '\\"'+source_path+"data/wide_eta_bin/file/save_v1/"+study+pattern.replace("/standard","")+run+extraText+"/histograms_data_incl_full.root"+'\\"'
                MC_file   = '\\"'+source_path+"MC/wide_eta_bin/file/"+study+pattern.replace("/standard","")+QCDsample+extraText+"/histograms_mc_incl_full.root"+'\\"'
                Data_file = '\\"'+source_path+"data/wide_eta_bin/file/"+study+pattern.replace("/standard","")+run+extraText+"/histograms_data_incl_full.root"+'\\"'
                # print MC_file, Data_file
                # if not os.path.isfile(str(MC_file.replace("\\","").strip("\""))) or not os.path.isfile(str(Data_file.replace("\\","").strip("\""))):
                #   continue
                # print MC_file, Data_file
                main_function(gaustails=False)

for i in list_processes:
  print i

print len(list_processes)
parallelise(list_processes, 1, list_logfiles)
