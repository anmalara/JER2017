#include "../include/parameters.h"
#include "../include/useful_functions.h"
#include "../include/CorrectionObject.h"
#include "../include/tdrstyle_mod15.h"

#include <TStyle.h>
#include <TF1.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TLine.h>
#include <TCanvas.h>
#include <TMatrixDSym.h>
#include <TPaveStats.h>
#include <TFitResult.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TVirtualFitter.h>
#include <TMath.h>
#include <TFile.h>
#include <TH2D.h>

using namespace std;

void CorrectionObject::AdditionalAsymmetryPlots(bool eta_abs, bool si_trg){
  cout << "--------------- StartingAdditionalAsymmetryPlots() ---------------" << endl << endl;
  gStyle->SetOptStat(0);

  int n_pt_ = max(n_pt,n_pt_HF);
  bool eta_cut_bool;
  int n_pt_cutted;
  
  // const int n_pt_ = (si_trg ? n_pt_Si : n_pt );

  // double pt_bins_[n_pt_]; = (si_trg ? pt_bins_Si : pt_bins);
  // TString pt_range_[n_pt_]; = (si_trg ? pt_range_Si : pt_range);

  // for(int i=0; i<n_pt_;i++)
  
  CorrectionObject::make_path(CorrectionObject::_outpath+"plots/control/fullAsym/");

  //Table with number of events in each pT- and eta-bin
  TH1D *hdata_met[n_pt_-1][(eta_abs ? n_eta : n_eta_full)  -1];
  TH1D *hmc_met[n_pt_-1][(eta_abs ? n_eta : n_eta_full)  -1];
  TH1D *hmc_asymmetry[n_pt_-1][(eta_abs ? n_eta : n_eta_full)  -1];
      
  TH1D *hdata_asymmetry[n_pt_-1][(eta_abs ? n_eta : n_eta_full)  -1]; // A for data
  TH1D *hdata_bsymmetry[n_pt_-1][(eta_abs ? n_eta : n_eta_full)  -1]; // B for data
  TH2D *hdata_asymmetry_nvert[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1]; // A for data
  TH2D *hdata_asymmetry_rho[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1]; // A for data

  TH1D *hdata_jet1_pt[(eta_abs ? n_eta : n_eta_full)-1];
  TH1D *hdata_jet2_pt[(eta_abs ? n_eta : n_eta_full)-1];  
  TH1D *hdata_jet3_pt[(eta_abs ? n_eta : n_eta_full)-1];
  
  TH1D *hdata_jet1_pt_binned[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1];
  TH1D *hdata_jet2_pt_binned[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1];  
  TH1D *hdata_jet3_pt_binned[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1];
  
  TH1D *hmc_jet1_pt_binned[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1];
  TH1D *hmc_jet2_pt_binned[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1];  
  TH1D *hmc_jet3_pt_binned[n_pt_-1][(eta_abs ? n_eta : n_eta_full)-1];
    
  TH1D *hdata_jet3_eta[(eta_abs ? n_eta : n_eta_full)-1];
  TH1D *hdata_jet3_dRmin[(eta_abs ? n_eta : n_eta_full)-1];//min dR to tag or probe jet from jet3
  
  int count = 0;
  TString name1 = "hist_data_A_";
  TString name2 = "hist_data_";
  TString name2mc = "hist_mc_";
  TString name3 = "hist_data_B_";

  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
      eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
      TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[j]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[j+1];

      hdata_jet1_pt[j] = new TH1D(name2+"jet1_pt_"+eta_name,"",nResponseBins,0,600);
      hdata_jet2_pt[j] = new TH1D(name2+"jet2_pt_"+eta_name,"",nResponseBins,0,600);    
      hdata_jet3_pt[j] = new TH1D(name2+"jet3_pt_"+eta_name,"",nResponseBins,0,600);
      hdata_jet3_eta[j] = new TH1D(name2+"jet3_eta_"+eta_name,"",100,-5.2,5.2);
      hdata_jet3_dRmin[j] = new TH1D(name2+"jet3_dRmin_"+eta_name,"",100,0,10);
      
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[k]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[k+1];

      TString name = name1 + eta_name + "_" + pt_name; 
      hdata_asymmetry[k][j] = new TH1D(name,"",nResponseBins, -1.2, 1.2);
      name = name3 + eta_name + "_" + pt_name; 
      hdata_bsymmetry[k][j] = new TH1D(name,"",nResponseBins, -1.2, 1.2);

      name = name1+"nvert_" + eta_name + "_" + pt_name;      
      hdata_asymmetry_nvert[k][j] = new TH2D(name,"",nResponseBins/2, -1.2, 1.2,nResponseBins/10 ,0,60);
      name = name1+"rho_" + eta_name + "_" + pt_name;    
      hdata_asymmetry_rho[k][j] = new TH2D(name,"",nResponseBins/2, -1.2, 1.2,nResponseBins/10 ,0,60);

      name = "hist_data_MET_" + eta_name + "_" + pt_name;
      hdata_met[k][j] = new TH1D(name,"",nResponseBins/8, 0, 300);

      name = "hist_mc_MET_" + eta_name + "_" + pt_name;
      hmc_met[k][j] = new TH1D(name,"",nResponseBins/8, 0, 300);

      name = name2+"jet1_pt"+eta_name + "_" + pt_name;
      hdata_jet1_pt_binned[k][j] = new TH1D(name,"",nResponseBins*3/8,(eta_cut_bool?pt_bins_HF:pt_bins)[k],(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]*3/2);
      name = name2+"jet2_pt"+eta_name + "_" + pt_name;
      hdata_jet2_pt_binned[k][j] = new TH1D(name,"",nResponseBins*3/8,0,(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]);      
      name = name2+"jet3_pt"+eta_name + "_" + pt_name;
      hdata_jet3_pt_binned[k][j] = new TH1D(name,"",nResponseBins*3/8,0,(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]);

      name = name2mc+"jet1_pt"+eta_name + "_" + pt_name;
      hmc_jet1_pt_binned[k][j] = new TH1D(name,"",nResponseBins*3/8,(eta_cut_bool?pt_bins_HF:pt_bins)[k],(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]*3/2);
      name = name2mc+"jet2_pt"+eta_name + "_" + pt_name;
      hmc_jet2_pt_binned[k][j] = new TH1D(name,"",nResponseBins*3/8,0,(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]);      
      name = name2mc+"jet3_pt"+eta_name + "_" + pt_name;
      hmc_jet3_pt_binned[k][j] = new TH1D(name,"",nResponseBins*3/8,0,(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]);
      
      count++;
    }
  }


  //Get relevant information from DATA, loop over DATA events
  TTreeReader myReader_DATA("AnalysisTree", CorrectionObject::_DATAFile);
  TTreeReaderValue<Float_t> met_data(myReader_DATA, "MET");
  TTreeReaderValue<Float_t> pt_ave_data(myReader_DATA, "pt_ave");
  TTreeReaderValue<Float_t> alpha_data(myReader_DATA, "alpha");
  TTreeReaderValue<Float_t> asymmetry_data(myReader_DATA, "asymmetry");
  TTreeReaderValue<Float_t> bsymmetry_data(myReader_DATA, "B");
  TTreeReaderValue<Float_t> probejet_eta_data(myReader_DATA, "probejet_eta");
  TTreeReaderValue<Float_t> probejet_pt_data(myReader_DATA, "probejet_pt");
  TTreeReaderValue<Float_t> weight_data(myReader_DATA, "weight");
  
  TTreeReaderValue<int> nvertices_data(myReader_DATA, "nvertices");
  TTreeReaderValue<Float_t> rho_data(myReader_DATA, "rho");
  
  TTreeReaderValue<Float_t> jet1_pt_data(myReader_DATA, "jet1_pt");
  TTreeReaderValue<Float_t> jet2_pt_data(myReader_DATA, "jet2_pt");
  TTreeReaderValue<Float_t> jet3_pt_data(myReader_DATA, "jet3_pt");
  TTreeReaderValue<Float_t> jet3_eta_data(myReader_DATA, "jet3_eta");
  TTreeReaderValue<Float_t> jet3_dRbarrel_data(myReader_DATA, "dR_jet3_barreljet");
  TTreeReaderValue<Float_t> jet3_dRprobe_data(myReader_DATA, "dR_jet3_probejet");
    
  int myCount = 0;
  int myCount_cut = 0;
  while (myReader_DATA.Next()) {
    if(*alpha_data>alpha_cut){
      myCount_cut++;
      continue;
    }
    //fill histos in bins of pt and eta

      for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
	eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      if(*pt_ave_data<(eta_cut_bool?pt_bins_HF:pt_bins)[k] || *pt_ave_data>(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]) continue;
	if( (eta_abs ? fabs(*probejet_eta_data) : *probejet_eta_data) > (eta_abs ? eta_bins : eta_bins_full)[j+1] || (eta_abs ? fabs(*probejet_eta_data) : *probejet_eta_data)  < (eta_abs ? eta_bins : eta_bins_full)[j]) continue;
	else{
	  hdata_jet1_pt[j]->Fill(*jet1_pt_data,*weight_data);
	  hdata_jet2_pt[j]->Fill(*jet2_pt_data,*weight_data);
	  hdata_jet3_pt[j]->Fill(*jet3_pt_data,*weight_data);
	  hdata_jet1_pt_binned[k][j]->Fill(*jet1_pt_data,*weight_data);
	  hdata_jet2_pt_binned[k][j]->Fill(*jet2_pt_data,*weight_data);
	  hdata_jet3_pt_binned[k][j]->Fill(*jet3_pt_data,*weight_data);	  
	  hdata_jet3_eta[j]->Fill(*jet3_eta_data,*weight_data);	  
	  double jet3_dRmin = *jet3_dRbarrel_data;
	  if(jet3_dRmin>*jet3_dRprobe_data) jet3_dRmin = *jet3_dRprobe_data;
	  hdata_jet3_dRmin[j]->Fill(jet3_dRmin,*weight_data);	  
	  hdata_asymmetry[k][j]->Fill(*asymmetry_data,*weight_data);
	  hdata_bsymmetry[k][j]->Fill(*bsymmetry_data,*weight_data);
	  hdata_asymmetry_rho[k][j]->Fill(*asymmetry_data,*rho_data,*weight_data);
	  hdata_asymmetry_nvert[k][j]->Fill(*asymmetry_data,*nvertices_data,*weight_data);
	  hdata_met[k][j]->Fill(*met_data,*weight_data);
	  myCount++;
	}
      }
    }
  }

  //DEBUG
  std::cout<<"\ncount data "<<myCount<<"  count cut data "<<myCount_cut<<std::endl<<std::endl;



  TTreeReader myReader_MC("AnalysisTree", CorrectionObject::_MCFile);
  TTreeReaderValue<Float_t> met_mc(myReader_MC, "MET");
  TTreeReaderValue<Float_t> jet1_pt_mc(myReader_MC, "jet1_pt");
  TTreeReaderValue<Float_t> jet2_pt_mc(myReader_MC, "jet2_pt");
  TTreeReaderValue<Float_t> jet3_pt_mc(myReader_MC, "jet3_pt");
  TTreeReaderValue<Float_t> pt_ave_mc(myReader_MC, "pt_ave");
  TTreeReaderValue<Float_t> alpha_mc(myReader_MC, "alpha");
      
  TTreeReaderValue<Float_t> probejet_eta_mc(myReader_MC, "probejet_eta");
  TTreeReaderValue<Float_t> probejet_pt_mc(myReader_MC, "probejet_pt");
  TTreeReaderValue<Float_t> weight_mc(myReader_MC, "weight");
  
  myCount = 0;
  myCount_cut = 0;
  while (myReader_MC.Next()) {
    if(*alpha_mc>alpha_cut){
      myCount_cut++;
      continue;
    }
    //fill histos in bins of pt and eta

      for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
	eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      if(*pt_ave_mc<(eta_cut_bool?pt_bins_HF:pt_bins)[k] || *pt_ave_mc>(eta_cut_bool?pt_bins_HF:pt_bins)[k+1]) continue;
	if( (eta_abs ? fabs(*probejet_eta_mc) : *probejet_eta_mc) > (eta_abs ? eta_bins : eta_bins_full)[j+1] || (eta_abs ? fabs(*probejet_eta_mc) : *probejet_eta_mc)  < (eta_abs ? eta_bins : eta_bins_full)[j]) continue;
	else{
	  hmc_jet1_pt_binned[k][j]->Fill(*jet1_pt_mc,*weight_mc);
	  hmc_jet2_pt_binned[k][j]->Fill(*jet2_pt_mc,*weight_mc);
	  hmc_jet3_pt_binned[k][j]->Fill(*jet3_pt_mc,*weight_mc);
	  hmc_met[k][j]->Fill(*met_mc,*weight_mc);

	  myCount++;
	}
      }
    }
  }

  //DEBUG
  std::cout<<"\ncount mc "<<myCount<<"  count cut mc "<<myCount_cut<<std::endl<<std::endl;

  

  
  TFile* test_out_data_A = new TFile(CorrectionObject::_outpath+"plots/control/A_1d_data_smaller_split.root","RECREATE");
  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      hdata_asymmetry[k][j]->Write();
      hdata_bsymmetry[k][j]->Write();
    }
  }
  test_out_data_A->Close();
  delete test_out_data_A;

  TFile* test_out_data_A_rho = new TFile(CorrectionObject::_outpath+"plots/control/A_rho_2d_data.root","RECREATE");
  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      hdata_asymmetry_rho[k][j]->Write();
    }
  }
  test_out_data_A_rho->Close();
  delete test_out_data_A_rho;
  

  TFile* test_out_data_A_nvert = new TFile(CorrectionObject::_outpath+"plots/control/A_nvert_2d_data.root","RECREATE");
  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      hdata_asymmetry_nvert[k][j]->Write();
    }
  }
  test_out_data_A_nvert->Close();
  delete test_out_data_A_nvert;

    TFile* test_out_data_jet_pt = new TFile(CorrectionObject::_outpath+"plots/control/jet_pt_data.root","RECREATE");
  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
      hdata_jet1_pt[j]->Write();
      hdata_jet2_pt[j]->Write();
      hdata_jet3_pt[j]->Write();     
      hdata_jet3_eta[j]->Write();     
      hdata_jet3_dRmin[j]->Write();
  }
  test_out_data_jet_pt->Close();
  delete test_out_data_jet_pt;
  
  TFile* test_out_data_mc_pt_binned = new TFile(CorrectionObject::_outpath+"plots/control/jet_pt_binned_data_mc.root","RECREATE");
  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      hdata_jet1_pt_binned[k][j]->Write();
      hdata_jet2_pt_binned[k][j]->Write();
      hdata_jet3_pt_binned[k][j]->Write();
      hmc_jet1_pt_binned[k][j]->Write();
      hmc_jet2_pt_binned[k][j]->Write();
      hmc_jet3_pt_binned[k][j]->Write();
    }
  }
  test_out_data_mc_pt_binned->Close();
  delete test_out_data_mc_pt_binned;
  
  TFile* test_out_data_mc_MET = new TFile(CorrectionObject::_outpath+"plots/control/MET_binned_data_mc.root","RECREATE");
  for(int j=0; j<(eta_abs ? n_eta : n_eta_full)-1; j++){
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[j])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      hdata_met[k][j]->Write();
      hmc_met[k][j]->Write();
    }
  }
  test_out_data_mc_MET->Close();
  delete test_out_data_mc_MET;

  // //R_MC and R_DATA overlaid in the same plot as a function of pT, in bins of |eta|
  // double val_rel_mc[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //value at pt,eta
  // double err_rel_mc[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //error of ratio at pt,eta
  // double val_mpf_mc[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //ratio at pt,eta
  // double err_mpf_mc[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //error of ratio at pt,eta
  // double val_rel_data[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //value at pt,eta
  // double err_rel_data[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //error of ratio at pt,eta
  // double val_mpf_data[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //ratio at pt,eta
  // double err_mpf_data[(eta_abs ? n_eta : n_eta_full)-1][n_pt-1]; //error of ratio at pt,eta

  // for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
  //   for(int j=0; j<n_pt-1; j++){

  //     //get <A> and error on <A>
  //     pair <double,double> A_data = GetValueAndError(hdata_asymmetry[j][i]);
  //     pair <double,double> B_data = GetValueAndError(hdata_bsymmetry[j][i]);
  //     //build MPF and pt_bal and their errors

  //   }
  // }

  //dummy for tdrCanvas
  TH1D *h = new TH1D("h",";dummy;",41,0,5.191);
  h->SetMaximum(1.2);
  h->SetMinimum(0.8);

  TH1D *hEF = new TH1D("hEF",";dummy;",1000,0,5.191);

  TCanvas* c_0 = new TCanvas();
  tdrCanvas(c_0,"c_0",h,4,10,true,CorrectionObject::_lumitag);

  
  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    //Create and fill TGraphErrors
    double xbin_tgraph[n_pt_-1];
    double zero[n_pt_-1];
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[i])>=eta_cut;
    for(int k=0; k<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); k++){
      xbin_tgraph[k]=(pt_bins[k]+pt_bins[k+1])/2;
      zero[k]=(pt_bins[k+1]-pt_bins[k])/2 ;
    }

    TString alVal;
    alVal.Form("%0.2f\n",alpha_cut);
    TString altitle = "{#alpha<"+alVal+"}";
    TString axistitle_mc = "R^{MC}_";
    TString axistitle_data = "R^{DATA}_";
 
     axistitle_data += altitle;

    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 

  }


  //********************************************************************  Plot all Control Hists ********************************************************************************

  // cout<<"before f_mpf file load"<<endl;

  //Get histo files
  TFile* f_mpf_mc = new TFile(CorrectionObject::_outpath+"plots/control/B_1d_mc" +".root","READ");
  TFile* f_mpf_data = new TFile(CorrectionObject::_outpath+"plots/control/B_1d_data"+".root","READ");
  
    for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TH1D* htemp_met_mc;
    
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      
     TCanvas* c5 = new TCanvas();
    tdrCanvas(c5,"c5",h,4,10,kSquare,"MC");
    // cout<<"after tdrCanvas"<<endl;
    
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_met_mc = "hist_mc_METoverJetsPt_"+eta_name+"_"+pt_name;
    
    htemp_met_mc = (TH1D*)f_mpf_mc->Get(name_met_mc);

    cout<<"after getting hist "<<name_met_mc<<endl;
    
    int n_ev =  htemp_met_mc->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    if(htemp_met_mc->Integral() > 0) htemp_met_mc->Scale(1/htemp_met_mc->Integral());

    TString textPt;
    textPt = text + ", "+legname;
    
    htemp_met_mc->GetXaxis()->SetTitle("MET/#sum p_{T}");
    htemp_met_mc->GetYaxis()->SetTitle("Norm. Entries");
    htemp_met_mc->GetYaxis()->SetTitleOffset(1.5);
    // h->SetMaximum(0.3);
    h->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_met_mc->SetMaximum(0.3);
    htemp_met_mc->SetLineWidth(3);

    // TF1 *tempFunc = new TF1("tempFunc","gaus");
    // htemp_met_mc->Fit(tempFunc);
    // TString textChi2NDF ="Chi2/NDF=";
    // textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetChisquare());
    // textChi2NDF = textChi2NDF+"/";
    // textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetNDF());    
      
    htemp_met_mc->Draw("HIST");

    tex->DrawLatex(0.2,0.85,"MC, " + textPt);
    // tex->DrawLatex(0.2,0.4,textChi2NDF);
    //tex_lumi->DrawLatex(0.6,0.91,"MC");
    c5->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/METoverPt_MC_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    
    }
  }

   for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TCanvas* c5 = new TCanvas();
    tdrCanvas(c5,"c5",h,4,10,kSquare,"DATA");
    TH1D* htemp_met_data;
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_met_data = "hist_data_METoverJetsPt_"+eta_name+"_"+pt_name;
    htemp_met_data = (TH1D*)f_mpf_data->Get(name_met_data);

    TString textPt;
    textPt = text + ", "+legname;
       
    int n_ev =  htemp_met_data->GetEntries();
    if(htemp_met_data->Integral() > 0)htemp_met_data->Scale(1/htemp_met_data->Integral());
    htemp_met_data->GetXaxis()->SetTitle("MET/#sum p_{T}");
    htemp_met_data->GetYaxis()->SetTitle("Norm. Entries");
    htemp_met_data->GetYaxis()->SetTitleOffset(1.5);
    // h->SetMaximum(0.3);
    htemp_met_data->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_met_data->SetMaximum(0.3);
    htemp_met_data->SetLineWidth(3);


    // TF1 *tempFunc = new TF1("tempFunc","gaus");
    // htemp_met_data->Fit(tempFunc);
    // TString textChi2NDF ="Chi2/NDF=";
    // textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetChisquare());
    // textChi2NDF = textChi2NDF+"/";
    // textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetNDF());    
    
    htemp_met_data->Draw("HIST");
    tex->DrawLatex(0.2,0.85,"DATA, " + textPt);
    // tex->DrawLatex(0.2,0.4,textChi2NDF);
    //tex_lumi->DrawLatex(0.6,0.91,"DATA");
    c5->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/METoverPt_DATA_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    
    }
  }


  
    for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TH1D* htemp_met_mc2;
    TH1D* htemp_met_data2;
    TH1D* htemp_met_datamc;
    
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      
     TCanvas* c52 = new TCanvas();
    tdrCanvas(c52,"c52",h,4,10,kSquare,"DATA/MC");
    // cout<<"after tdrCanvas"<<endl;
    
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_met_mc = "hist_mc_METoverJetsPt_"+eta_name+"_"+pt_name;
    TString name_met_data = "hist_data_METoverJetsPt_"+eta_name+"_"+pt_name;
    
    htemp_met_mc2 = (TH1D*)f_mpf_mc->Get(name_met_mc);
    htemp_met_data2 = (TH1D*)f_mpf_data->Get(name_met_data);
    
    cout<<"after getting hist "<<name_met_mc<<endl;
    cout<<"and "<<name_met_data<<endl;
    
    int n_ev =  htemp_met_mc2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    if(htemp_met_mc2->Integral() > 0) htemp_met_mc2->Scale(1/htemp_met_mc2->Integral());

    n_ev =  htemp_met_data2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    if(htemp_met_data2->Integral() > 0) htemp_met_data2->Scale(1/htemp_met_data2->Integral());   

    htemp_met_datamc = (TH1D*)htemp_met_data2->Clone();
    htemp_met_datamc->Divide(htemp_met_mc2);
    
    TString textPt;
    textPt = text + ", "+legname;
    
    htemp_met_mc2->GetXaxis()->SetTitle("MET/#sum p_{T}");
    htemp_met_mc2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_met_mc2->GetYaxis()->SetTitleOffset(1.5);
    htemp_met_data2->GetXaxis()->SetTitle("MET/#sum p_{T}");
    htemp_met_data2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_met_data2->GetYaxis()->SetTitleOffset(1.5);

    
    // h->SetMaximum(0.3);
    h->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_met_mc->SetMaximum(0.3);


    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
      
    htemp_met_mc2->SetLineWidth(3);
    htemp_met_mc2->Draw("HIST");
    htemp_met_data2->SetLineWidth(3);
    htemp_met_data2->Draw("* same");
    
    tex->DrawLatex(0.2,0.85,textPt);

    c52->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    
    htemp_met_datamc->Draw();
        
    c52->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/METoverPt_DataMC_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    
    }
  }



   

   
  TFile* f_jet_pt = new TFile(CorrectionObject::_outpath+"plots/control/jet_pt_data.root","READ");
  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045); 
    
      TCanvas* cFull_pt1 = new TCanvas();
      tdrCanvas(cFull_pt1,"cFull_pt1",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_rel_data;
      TString name_rel_data = "hist_data_jet1_pt_"+eta_name;
      htemp_rel_data = (TH1D*)f_jet_pt->Get(name_rel_data);
      htemp_rel_data->Draw();
      htemp_rel_data->GetXaxis()->SetTitle("pt jet1");
      htemp_rel_data->GetYaxis()->SetTitle("Entries per Bin");      
      // htemp_rel_data->GetXaxis()->SetLimits(-1.2,1.2);


    TF1 *tempFunc = new TF1("tempFunc","gaus");
    htemp_rel_data->Fit(tempFunc);
    TString textChi2NDF ="Chi2/NDF=";
    textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetChisquare());
    textChi2NDF = textChi2NDF+"/";
    textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetNDF());    

      htemp_rel_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
    tex->DrawLatex(0.2,0.4,textChi2NDF);
      cFull_pt1->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/jet1_pt_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+ ".pdf");
      delete cFull_pt1;
      delete htemp_rel_data;
    
  }

  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    //TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];

    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045); 
    
      TCanvas* cFull_pt2 = new TCanvas();
      tdrCanvas(cFull_pt2,"cFull_pt2",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_rel_data;
      TString name_rel_data = "hist_data_jet2_pt_"+eta_name;
      htemp_rel_data = (TH1D*)f_jet_pt->Get(name_rel_data);
      htemp_rel_data->Draw();
      htemp_rel_data->GetXaxis()->SetTitle("pt jet2");
      htemp_rel_data->GetYaxis()->SetTitle("Entries per Bin");      
      // htemp_rel_data->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      cFull_pt2->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/jet2_pt_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+ ".pdf");
      delete cFull_pt2;
      delete htemp_rel_data;
    
  }

  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 

    //TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045); 
    
      TCanvas* cFull_pt3 = new TCanvas();
      tdrCanvas(cFull_pt3,"cFull_pt3",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_rel_data;
      TString name_rel_data = "hist_data_jet3_pt_"+eta_name;
      htemp_rel_data = (TH1D*)f_jet_pt->Get(name_rel_data);
      htemp_rel_data->Draw();
      htemp_rel_data->GetXaxis()->SetTitle("pt jet3");
      htemp_rel_data->GetYaxis()->SetTitle("Entries per Bin");      
      // htemp_rel_data->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      cFull_pt3->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/jet3_pt_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+ ".pdf");
      delete cFull_pt3;
      delete htemp_rel_data;
    
  }  


  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    //    TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045); 
    
      TCanvas* cFull_eta3 = new TCanvas();
      tdrCanvas(cFull_eta3,"cFull_eta3",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_rel_data;
      TString name_rel_data = "hist_data_jet3_eta_"+eta_name;
      htemp_rel_data = (TH1D*)f_jet_pt->Get(name_rel_data);
      htemp_rel_data->Draw();
      htemp_rel_data->GetXaxis()->SetTitle("eta jet3");
      htemp_rel_data->GetYaxis()->SetTitle("Entries per Bin");      
      // htemp_rel_data->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data->SetMarkerStyle(20);
      htemp_rel_data->SetMarkerColor(kBlack);
      htemp_rel_data->SetLineColor(kBlack);
      htemp_rel_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      cFull_eta3->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/jet3_eta_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+ ".pdf");
      delete cFull_eta3;
      delete htemp_rel_data;
    
  }  

for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    //    TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045); 
    
      TCanvas* cFull_dRmin3 = new TCanvas();
      tdrCanvas(cFull_dRmin3,"cFull_dRmin3",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_rel_data;
      TString name_rel_data = "hist_data_jet3_dRmin_"+eta_name;
      htemp_rel_data = (TH1D*)f_jet_pt->Get(name_rel_data);
      htemp_rel_data->Draw();
      htemp_rel_data->GetXaxis()->SetTitle("min (dR(jet3,tag), dR(jet3,probe))");
      htemp_rel_data->GetYaxis()->SetTitle("Entries per Bin");      
      // htemp_rel_data->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data->SetMarkerStyle(20);
      htemp_rel_data->SetMarkerColor(kBlack);
      htemp_rel_data->SetLineColor(kBlack);
      htemp_rel_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      cFull_dRmin3->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/jet3_dRmin_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+ ".pdf");
      delete cFull_dRmin3;
      delete htemp_rel_data;
    
  }  

   TFile* f_rel_data_nvert = new TFile(CorrectionObject::_outpath+"plots/control/A_nvert_2d_data.root","READ");
  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    //    TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045); 
    
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[i])>=eta_cut;
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      TCanvas* cFullA_nvert = new TCanvas();
      // tdrCanvas(cFullA_nvert,"cFullA_nvert",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH2D* htemp_rel_data_nvert;
      TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
      TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
      TString name_rel_data = "hist_data_A_nvert_"+eta_name+"_"+pt_name;
      htemp_rel_data_nvert = (TH2D*)f_rel_data_nvert->Get(name_rel_data);
      htemp_rel_data_nvert->Draw("E");
      htemp_rel_data_nvert->GetXaxis()->SetTitle("A");
      htemp_rel_data_nvert->GetYaxis()->SetTitle("n vertices");
      htemp_rel_data_nvert->GetZaxis()->SetTitle("Entries per Bin");      
      htemp_rel_data_nvert->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data_nvert->Draw("COLZ");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      tex->DrawLatex(0.54,0.8,legname);		
      cFullA_nvert->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/A_nvert_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+"_pt_"+ (eta_cut_bool?pt_range_HF:pt_range)[j] + "_" + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + ".pdf");
      delete cFullA_nvert;
      delete htemp_rel_data_nvert;
    }
  }
 
  TFile* f_rel_data = new TFile(CorrectionObject::_outpath+"plots/control/A_1d_data_smaller_split"+".root","READ");
  //  f_rel_data->Print();
  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    //    TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
    
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[i])>=eta_cut;
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      TCanvas* cFullA = new TCanvas();
      tdrCanvas(cFullA,"cFullA",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_rel_data;
      TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
      TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
      TString evname = "Tot.Events = ";
      
      TF1 *tempFunc = new TF1("tempFunc","gaus");
	 
      TString name_rel_data = "hist_data_A_"+eta_name+"_"+pt_name;
      htemp_rel_data = (TH1D*)f_rel_data->Get(name_rel_data);
      evname += htemp_rel_data->GetEntries();
      htemp_rel_data->Draw("E");
	TString textChi2NDF ="Chi2/NDF=";
      if(htemp_rel_data->GetEntries()){
	htemp_rel_data->Fit(tempFunc);
	textChi2NDF = textChi2NDF+Form("%.1f",tempFunc->GetChisquare());
	textChi2NDF = textChi2NDF+"/";
	textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetNDF());
      }
      htemp_rel_data->GetXaxis()->SetTitle("A");
      htemp_rel_data->GetYaxis()->SetTitle("Entries per Bin");
      htemp_rel_data->GetYaxis()->SetTitleOffset(1.5);
      htemp_rel_data->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      tex->DrawLatex(0.54,0.8,legname);		
      tex->DrawLatex(0.58,0.75,evname);
      if(htemp_rel_data->GetEntries()) tex->DrawLatex(0.47,0.6,textChi2NDF);
    
      
      cFullA->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/A_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+"_pt_"+ (eta_cut_bool?pt_range_HF:pt_range)[j] + "_" + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + ".pdf");
      delete cFullA;
      delete htemp_rel_data;
      
      tempFunc = new TF1("tempFunc","gaus");
      TCanvas* cFullB = new TCanvas();
      tdrCanvas(cFullB,"cFullB",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH1D* htemp_mpf_data;
      TString name_mpf_data = "hist_data_B_"+eta_name+"_"+pt_name;
      htemp_mpf_data = (TH1D*)f_rel_data->Get(name_mpf_data);
      evname = "Tot.Events = ";
      evname += htemp_mpf_data->GetEntries();
      htemp_mpf_data->Draw("E");
      textChi2NDF ="Chi2/NDF=";
      if(htemp_mpf_data->GetEntries()){
	htemp_mpf_data->Fit(tempFunc);
	textChi2NDF = textChi2NDF+Form("%.1f",tempFunc->GetChisquare());
	textChi2NDF = textChi2NDF+"/";
	textChi2NDF = textChi2NDF+Form("%d",tempFunc->GetNDF());
      }
      htemp_mpf_data->GetXaxis()->SetTitle("B");
      htemp_mpf_data->GetYaxis()->SetTitle("Entries per Bin");
      htemp_mpf_data->GetYaxis()->SetTitleOffset(1.5);
      htemp_mpf_data->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_mpf_data->SetMarkerColor(kRed);
      htemp_mpf_data->SetMarkerStyle(21);
      htemp_mpf_data->Draw("EP");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      tex->DrawLatex(0.54,0.8,legname);		
      tex->DrawLatex(0.58,0.75,evname);	
       if(htemp_mpf_data->GetEntries()) tex->DrawLatex(0.47,0.6,textChi2NDF);	
      cFullB->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/B_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+"_pt_"+ (eta_cut_bool?pt_range_HF:pt_range)[j] + "_" + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + ".pdf");
    
      delete cFullB;
      delete htemp_mpf_data;


    }
  }

  TFile* f_rel_data_rho = new TFile(CorrectionObject::_outpath+"plots/control/A_rho_2d_data.root","READ");
  cout<<"Read rho root file"<<endl;
  for(int i=0; i<(eta_abs ? n_eta : n_eta_full)-1; i++){
    TString eta_name = "eta_"+(eta_abs ? eta_range2 : eta_range2_full)[i]+"_"+(eta_abs ? eta_range2 : eta_range2_full)[i+1];
    
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    //    TString text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
    
    eta_cut_bool = fabs((eta_abs ? eta_bins : eta_bins_full)[i])>=eta_cut;
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      TCanvas* cFullA_rho = new TCanvas();
      // tdrCanvas(cFullA_rho,"cFullA_rho",h,4,10,kSquare,CorrectionObject::_lumitag);
      TH2D* htemp_rel_data_rho;
      TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
      TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
      TString name_rel_data = "hist_data_A_rho_"+eta_name+"_"+pt_name;
      htemp_rel_data_rho = (TH2D*)f_rel_data_rho->Get(name_rel_data);
      htemp_rel_data_rho->Draw("E");
      htemp_rel_data_rho->GetXaxis()->SetTitle("A");
      htemp_rel_data_rho->GetYaxis()->SetTitle("rho");
      htemp_rel_data_rho->GetZaxis()->SetTitle("Entries per Bin");      
      htemp_rel_data_rho->GetXaxis()->SetLimits(-1.2,1.2);
      htemp_rel_data_rho->Draw("COLZ");		
      tex->DrawLatex(0.47,0.85,"Data, " + text);
      tex->DrawLatex(0.54,0.8,legname);		
      cFullA_rho->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/A_rho_DATA_" + CorrectionObject::_generator_tag + "_eta_" + (eta_abs ? eta_range2 : eta_range2_full)[i] + "_" + (eta_abs ? eta_range2 : eta_range2_full)[i+1]+"_pt_"+ (eta_cut_bool?pt_range_HF:pt_range)[j] + "_" + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + ".pdf");
      delete cFullA_rho;
      delete htemp_rel_data_rho;
    }
  }


  TFile* f_pt_binned_data_mc = new TFile(CorrectionObject::_outpath+"plots/control/jet_pt_binned_data_mc" +".root","READ");
  
    for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TH1D* htemp_pt_mc2;
    TH1D* htemp_pt_data2;
    TH1D* htemp_pt_datamc;
    
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      
     TCanvas* c_ptbinned = new TCanvas();
    tdrCanvas(c_ptbinned,"c_ptbinned",h,4,10,kSquare,"DATA/MC");
    // cout<<"after tdrCanvas"<<endl;
    
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_pt_mc = "hist_mc_jet1_pt"+eta_name+"_"+pt_name;
    TString name_pt_data = "hist_data_jet1_pt"+eta_name+"_"+pt_name;
    
    htemp_pt_mc2 = (TH1D*)f_pt_binned_data_mc->Get(name_pt_mc);
    htemp_pt_data2 = (TH1D*)f_pt_binned_data_mc->Get(name_pt_data);
    
    cout<<"after getting hist "<<name_pt_mc<<endl;
    cout<<"and "<<name_pt_data<<endl;
    
    int n_ev =  htemp_pt_mc2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_pt_mc2->Integral() > 0) htemp_pt_mc2->Scale(1/htemp_pt_mc2->Integral());

    n_ev =  htemp_pt_data2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_pt_data2->Integral() > 0) htemp_pt_data2->Scale(1/htemp_pt_data2->Integral());   

    htemp_pt_datamc = (TH1D*)htemp_pt_data2->Clone();
    htemp_pt_datamc->Divide(htemp_pt_mc2);
    
    TString textPt;
    textPt = text + ", "+legname;
    
    htemp_pt_mc2->GetXaxis()->SetTitle("jet 1 p_{T}");
    htemp_pt_mc2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_pt_mc2->GetYaxis()->SetTitleOffset(1.5);
    htemp_pt_data2->GetXaxis()->SetTitle("jet 1 p_{T}");
    htemp_pt_data2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_pt_data2->GetYaxis()->SetTitleOffset(1.5);

    
    // h->SetMaximum(0.3);
    h->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_pt_mc->SetMaximum(0.3);


    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
      
    htemp_pt_mc2->SetLineWidth(3);
    htemp_pt_mc2->Draw("HIST");
    htemp_pt_data2->SetLineWidth(3);
    htemp_pt_data2->SetLineColor(kBlack);
    htemp_pt_data2->Draw("* same");
    
    tex->DrawLatex(0.2,0.85,textPt);

    c_ptbinned->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    htemp_pt_datamc->GetYaxis()->SetTitle("Ratio");
    htemp_pt_datamc->GetYaxis()->SetTitleOffset(-0.2);
    htemp_pt_datamc->GetXaxis()->SetTitle("jet 1 p_{T}");
    htemp_pt_datamc->SetMaximum(2.);
    htemp_pt_datamc->SetMinimum(0.5);
    htemp_pt_datamc->SetLabelSize(0.12,"xyz");
    htemp_pt_datamc->SetTitleSize(0.12,"xyz");
    htemp_pt_datamc->SetLineWidth(3);
    htemp_pt_datamc->Draw();
        
    c_ptbinned->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/Pt_jet_1_binned_DataMC_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    delete c_ptbinned;
    delete htemp_pt_mc2;
    delete htemp_pt_data2;
    delete htemp_pt_datamc;
    
    }
  }


    for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TH1D* htemp_pt_mc2;
    TH1D* htemp_pt_data2;
    TH1D* htemp_pt_datamc;
    
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      
     TCanvas* c_ptbinned = new TCanvas();
    tdrCanvas(c_ptbinned,"c_ptbinned",h,4,10,kSquare,"DATA/MC");
    // cout<<"after tdrCanvas"<<endl;
    
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_pt_mc = "hist_mc_jet2_pt"+eta_name+"_"+pt_name;
    TString name_pt_data = "hist_data_jet2_pt"+eta_name+"_"+pt_name;
    
    htemp_pt_mc2 = (TH1D*)f_pt_binned_data_mc->Get(name_pt_mc);
    htemp_pt_data2 = (TH1D*)f_pt_binned_data_mc->Get(name_pt_data);
    
    cout<<"after getting hist "<<name_pt_mc<<endl;
    cout<<"and "<<name_pt_data<<endl;
    
    int n_ev =  htemp_pt_mc2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_pt_mc2->Integral() > 0) htemp_pt_mc2->Scale(1/htemp_pt_mc2->Integral());

    n_ev =  htemp_pt_data2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_pt_data2->Integral() > 0) htemp_pt_data2->Scale(1/htemp_pt_data2->Integral());   

    htemp_pt_datamc = (TH1D*)htemp_pt_data2->Clone();
    htemp_pt_datamc->Divide(htemp_pt_mc2);
    
    TString textPt;
    textPt = text + ", "+legname;
    
    htemp_pt_mc2->GetXaxis()->SetTitle("jet 2 p_{T}");
    htemp_pt_mc2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_pt_mc2->GetYaxis()->SetTitleOffset(1.5);
    htemp_pt_data2->GetXaxis()->SetTitle("jet 2 p_{T}");
    htemp_pt_data2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_pt_data2->GetYaxis()->SetTitleOffset(1.5);

    
    // h->SetMaximum(0.3);
    h->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_pt_mc->SetMaximum(0.3);


    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
      
    htemp_pt_mc2->SetLineWidth(3);
    htemp_pt_mc2->Draw("HIST");
    htemp_pt_data2->SetLineWidth(3);
    htemp_pt_data2->SetLineColor(kBlack);
    htemp_pt_data2->Draw("* same");
    
    tex->DrawLatex(0.2,0.85,textPt);

    c_ptbinned->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    htemp_pt_datamc->GetYaxis()->SetTitle("Ratio");
    htemp_pt_datamc->GetYaxis()->SetTitleOffset(-0.2);
    htemp_pt_datamc->GetXaxis()->SetTitle("jet 2 p_{T}");
    htemp_pt_datamc->SetMaximum(2.);
    htemp_pt_datamc->SetMinimum(0.5);
    htemp_pt_datamc->SetLabelSize(0.12,"xyz");
    htemp_pt_datamc->SetTitleSize(0.12,"xyz");
    htemp_pt_datamc->SetLineWidth(3);
    htemp_pt_datamc->Draw();
        
    c_ptbinned->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/Pt_jet_2_binned_DataMC_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    delete c_ptbinned;
    delete htemp_pt_mc2;
    delete htemp_pt_data2;    
    delete htemp_pt_datamc;
    
    }
  }

    for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TH1D* htemp_pt_mc2;
    TH1D* htemp_pt_data2;
    TH1D* htemp_pt_datamc;
    
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      
     TCanvas* c_ptbinned = new TCanvas();
    tdrCanvas(c_ptbinned,"c_ptbinned",h,4,10,kSquare,"DATA/MC");
    // cout<<"after tdrCanvas"<<endl;
    
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_pt_mc = "hist_mc_jet3_pt"+eta_name+"_"+pt_name;
    TString name_pt_data = "hist_data_jet3_pt"+eta_name+"_"+pt_name;
    
    htemp_pt_mc2 = (TH1D*)f_pt_binned_data_mc->Get(name_pt_mc);
    htemp_pt_data2 = (TH1D*)f_pt_binned_data_mc->Get(name_pt_data);
    
    cout<<"after getting hist "<<name_pt_mc<<endl;
    cout<<"and "<<name_pt_data<<endl;
    
    int n_ev =  htemp_pt_mc2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_pt_mc2->Integral() > 0) htemp_pt_mc2->Scale(1/htemp_pt_mc2->Integral());

    n_ev =  htemp_pt_data2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_pt_data2->Integral() > 0) htemp_pt_data2->Scale(1/htemp_pt_data2->Integral());   

    htemp_pt_datamc = (TH1D*)htemp_pt_data2->Clone();
    htemp_pt_datamc->Divide(htemp_pt_mc2);
    
    TString textPt;
    textPt = text + ", "+legname;
    
    htemp_pt_mc2->GetXaxis()->SetTitle("jet 3 p_{T}");
    htemp_pt_mc2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_pt_mc2->GetYaxis()->SetTitleOffset(1.5);
    htemp_pt_data2->GetXaxis()->SetTitle("jet 3 p_{T}");
    htemp_pt_data2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_pt_data2->GetYaxis()->SetTitleOffset(1.5);

    
    // h->SetMaximum(0.3);
    h->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_pt_mc->SetMaximum(0.3);


    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
      
    htemp_pt_mc2->SetLineWidth(3);
    htemp_pt_mc2->Draw("HIST");
    htemp_pt_data2->SetLineWidth(3);
    htemp_pt_data2->SetLineColor(kBlack);
    htemp_pt_data2->Draw("* same");
    
    tex->DrawLatex(0.2,0.85,textPt);

    c_ptbinned->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    htemp_pt_datamc->GetYaxis()->SetTitle("Ratio");
    htemp_pt_datamc->GetYaxis()->SetTitleOffset(-0.2);
    htemp_pt_datamc->GetXaxis()->SetTitle("jet 3 p_{T}");
    htemp_pt_datamc->SetMaximum(2.);
    htemp_pt_datamc->SetMinimum(0.5);
    htemp_pt_datamc->SetLabelSize(0.12,"xyz");
    htemp_pt_datamc->SetTitleSize(0.12,"xyz");
    htemp_pt_datamc->SetLineWidth(3);
    htemp_pt_datamc->Draw();
        
    c_ptbinned->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/Pt_jet_3_binned_DataMC_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    delete c_ptbinned;
    delete htemp_pt_mc2;
    delete htemp_pt_data2;    
    delete htemp_pt_datamc;
    
    }
  }
   


  TFile* f_met_binned_data_mc = new TFile(CorrectionObject::_outpath+"plots/control/MET_binned_data_mc" +".root","READ");
  
    for(int i=0; i<n_eta-1; i++){
      TString eta_name = "eta_"+eta_range2[i]+"_"+eta_range2[i+1];
    bool eta_cut_bool = fabs(eta_bins[i])>=eta_cut; 
    TLatex *tex = new TLatex();
    tex->SetNDC();
    tex->SetTextSize(0.045); 
    TString text;
    if(eta_abs) text = eta_range[i] + " < |#eta| < " + eta_range[i+1];
    else text = eta_range_full[i] + " < #eta < " + eta_range_full[i+1];

    TLatex *tex_lumi = new TLatex();
    tex_lumi->SetNDC();
    tex_lumi->SetTextSize(0.045);
      
    TH1D* htemp_met_mc2;
    TH1D* htemp_met_data2;
    TH1D* htemp_met_datamc;
    
    for(int j=0; j<( eta_cut_bool ?  n_pt_HF-1 : n_pt-1 ); j++){
      
     TCanvas* c_metbinned = new TCanvas();
    tdrCanvas(c_metbinned,"c_metbinned",h,4,10,kSquare,"DATA/MC");
    // cout<<"after tdrCanvas"<<endl;
    
    TString pt_name = "pt_"+(eta_cut_bool?pt_range_HF:pt_range)[j]+"_"+(eta_cut_bool?pt_range_HF:pt_range)[j+1];
    TString legname = "p_{T} #in [" + (eta_cut_bool?pt_range_HF:pt_range)[j] + "," + (eta_cut_bool?pt_range_HF:pt_range)[j+1] + "]";
    TString name_met_mc = "hist_mc_MET_"+eta_name+"_"+pt_name;
    TString name_met_data = "hist_data_MET_"+eta_name+"_"+pt_name;
    
    htemp_met_mc2 = (TH1D*)f_met_binned_data_mc->Get(name_met_mc);
    htemp_met_data2 = (TH1D*)f_met_binned_data_mc->Get(name_met_data);
    
    cout<<"after getting hist "<<name_met_mc<<endl;
    cout<<"and "<<name_met_data<<endl;
    int n_ev =  htemp_met_mc2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_met_mc2->Integral() > 0) htemp_met_mc2->Scale(1/htemp_met_mc2->Integral());

    n_ev =  htemp_met_data2->GetEntries();
    cout<<"hist has "<<n_ev<<" entries"<<endl;

    // if(htemp_met_data2->Integral() > 0) htemp_met_data2->Scale(1/htemp_met_data2->Integral());   

    htemp_met_datamc = (TH1D*)htemp_met_data2->Clone();
    htemp_met_datamc->Divide(htemp_met_mc2);
    
    TString textPt;
    textPt = text + ", "+legname;
    
    htemp_met_mc2->GetXaxis()->SetTitle("E_{T}^{miss}");
    htemp_met_mc2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_met_mc2->GetYaxis()->SetTitleOffset(1.5);
    htemp_met_data2->GetXaxis()->SetTitle("E_{T}^{miss}");
    htemp_met_data2->GetYaxis()->SetTitle("Norm. Entries");
    htemp_met_data2->GetYaxis()->SetTitleOffset(1.5);

    
    // h->SetMaximum(0.3);
    h->GetXaxis()->SetLimits(0,1.2);
    //      h->GetYaxis()->SetLimits(0,0.8);
    // htemp_met_mc->SetMaximum(0.3);


    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
      
    htemp_met_mc2->SetLineWidth(3);
    htemp_met_mc2->Draw("HIST");
    htemp_met_data2->SetLineWidth(3);
    htemp_met_data2->SetLineColor(kBlack);
    htemp_met_data2->Draw("* same");
    
    tex->DrawLatex(0.2,0.85,textPt);

    c_metbinned->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();
    htemp_met_datamc->GetYaxis()->SetTitle("Ratio");
    htemp_met_datamc->GetYaxis()->SetTitleOffset(-0.2);
    htemp_met_datamc->GetXaxis()->SetTitle("E_{T}^{miss}");
    htemp_met_datamc->SetMaximum(2.);
    htemp_met_datamc->SetMinimum(0.5);
    htemp_met_datamc->SetLabelSize(0.12,"xyz");
    htemp_met_datamc->SetTitleSize(0.12,"xyz");
    htemp_met_datamc->SetLineWidth(3);
    htemp_met_datamc->Draw();
        
    c_metbinned->SaveAs(CorrectionObject::_outpath+"plots/control/fullAsym/Met_binned_DataMC_" + CorrectionObject::_generator_tag + "_eta_" + eta_range2[i] + "_" + eta_range2[i+1] + "_" + pt_name + ".pdf");
    delete c_metbinned;
    delete htemp_met_mc2;
    delete htemp_met_data2;
    delete htemp_met_datamc;
    
    }
  }


    

}
    
    


