/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THIS IS A FILE THAT TAKES AS INPUT ROOT VARIABLE FILES
// SCALING FACTORS:
// process     : cross-section (mb)
// qq_Z_ee     : 1.519e-06 (signal)
// qq_gm_ee    : 3.577e-05 (bkg)
// qq_Z_tata   : 1.514e-06 (bkg) * [BR(ta -> e nu nu)]^2 = 0.031429333
// tt_WW       : 74e-10 (bkg) <- composed
// Composed branching ratios:
// [BR(t -> W b)]^2 = 0.9964
// [BR(W -> e nu)]^2 = 0.011664

// RUN LIKE THIS:
// SIGNAL FIRST, REST BACKGROUND
/*
root -l -b -q stack_hist.C'({
"processes/qq_Z_ee/final.root"
, "processes/qq_gm_ee/final.root"
, "processes/qq_Z_tata/final.root"
,"processes/tt_WW/final.root"}
, {1.519e-06, 3.577e-05, 4.76e-08, 8.62e-11}
, {1e6      , 100e6    , 1e6      , 3e6})'
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <cstring>
#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <THStack.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <map>
#include <TColor.h>

std::string approx_toKM(int n) {
    if (n >= 1000000) {
        return std::to_string((n + 500000) / 1000000) + "M";
    } else if (n >= 1000) {
        return std::to_string((n + 500) / 1000) + "K";
    } else {
        return std::to_string(n);
    }
}

void stack_hist(std::vector<std::string> inputFiles, std::vector<double> crossSection, std::vector<double> TotalEvents) {
    Double_t Lint_total = 0;
    int nFiles = inputFiles.size();

    /////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////// INPUTS ///////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    // Canvas Size
    int XPixels = 1700;
    int YPixels = 1000;
    // Margins
    Double_t TopMargin = 0.13;   
    Double_t BottomMargin = 0.15; 
    Double_t LeftMargin = 0.10;   
    Double_t RightMargin = 0.03;
    // Text Size
    Double_t TextSize = 0.06;
    // Axis Title Offsets
    Double_t XTitleOffset = 1.2; 
    Double_t YTitleOffset = 0.9; 
    // Max events to process
    Int_t N_events = 1e7; // A very large number --change if you like

    // Target luminosity (that of the signal)
    Double_t L_target = 288858 / crossSection[0]; // in mb^{-1}

    Double_t top_margin = 2; // Top margin for y-axis (multiplicative)
    
    int CrossXUnit = 1; // 0 - mb (default)
    				    // 1 - fb
    					  
    int NORM = 0; // 0 - - multiply by crossx each hist (default)
    			  // 1 - normalize to Lint?
    				   
    int optLOGX = 0; // 0 - linear y scale
                     // 1 - log y scale
    int optLOGY = 1; // 0 - linear y scale
                     // 1 - log y scale

    // Canvas color
    int CanvasCOLOR = 1; // 0 - white canvas (default)
                         // 1 - black canvas
    // Y-ranges
	Double_t range_y = 0;

	// Final Color Vector for stack on a black canvas
	const int DeepElectricBlue = TColor::GetColor(20, 90, 165);
	const int DeeperCrimsonRed = TColor::GetColor(150, 20, 40);
	const int DeepNeonGreen    = TColor::GetColor(0, 180, 90);
    const int BrightChestnut   = TColor::GetColor(180, 90, 30);

	const int GoldenSunset     = TColor::GetColor(255, 165, 0);   // warm gold-orange
	const int MysticPurple     = TColor::GetColor(130, 50, 200);  // deep electric purple
	const int IceBlueGlacier   = TColor::GetColor(100, 220, 255); // cold icy blue
	const int Black  = kBlack;
	const int Yellow = kYellow;
	const int Red    = kRed;
	const int Blue   = kBlue;


	std::vector<int> colors = {
		//DeeperCrimsonRed,
		//DeepElectricBlue,
		//DeepNeonGreen,
		//GoldenSunset,
		//MysticPurple,
		//BrightChestnut,
		//IceBlueGlacier
		kCyan,
		kOrange+2,
		kPink-4,
		kBlack
	};

    // Legend File Names Transformation
    std::vector<std::string> legendNames(nFiles);
    for (int i = 0; i < nFiles; ++i) {
        std::string fileName = inputFiles[i];
        if (fileName == "processes/qq_Z_ee/final.root") fileName = "q #bar{q} #rightarrow Z";
        if (fileName == "processes/qq_gm_ee/final.root") fileName = "q #bar{q} #rightarrow #gamma*";
        if (fileName == "processes/qq_Z_tata/final.root") fileName = "q #bar{q} #rightarrow Z #rightarrow #tau^{-} #tau^{+}";
        if (fileName == "processes/tt_WW/final.root") fileName = "W^{-}W^{+}/t#bar{t}";
        legendNames[i] = fileName;
    }
    
    // X-ranges
    std::map<std::string, std::pair<double, double>> ranges_x = {
		{"DielectronMass",         {10, 300}},
		{"Dielectron_p4.E",        {20, 600}},
		{"DielectronDeltaR",       {1e-1, 7}},
		{"Dielectron_p4.Pt",       {0.01, 9}},
		{"Dielectron_p4.Px",       {1e-1, 1}},
		{"Dielectron_p4.Py",       {1e-1, 1}},
		{"Dielectron_p4.Pz",       {1e-2, 500}},
		{"Dielectron_angle.eta",   {-9, +9}},
		{"Dielectron_angle.phi",   {0, 2*TMath::Pi()}},
		{"Dielectron_angle.theta", {1e-4, TMath::Pi()}},
		{"Actual_phi",             {2.65, 3.55}}
	};
	// X-density 
	std::map<std::string, double> BinSize = {
		{"DielectronMass",         2},
		{"Dielectron_p4.E",        1},
		{"DielectronDeltaR",       0.5},
		{"Dielectron_p4.Pt",       0.02},
		{"Dielectron_p4.Px",       0.1},
		{"Dielectron_p4.Py",       0.1},
		{"Dielectron_p4.Pz",       0.05},
		{"Dielectron_angle.eta",   0.1},
		{"Dielectron_angle.phi",   0.05},
		{"Dielectron_angle.theta", 0.0001},
		{"Actual_phi",             0.001}
	};
    // Set branch title
    std::map<std::string, std::string> branchTitles = {
        {"DielectronMass",         "m_{(ee)} [GeV]"},
        {"Dielectron_p4.E",        "E_{(ee)} [GeV]"},
        {"Dielectron_p4.Px",       "p_{x(ee)} [GeV]"},
        {"Dielectron_p4.Py",       "p_{y(ee)} [GeV]"},
        {"Dielectron_p4.Pz",       "p_{z(ee)} [GeV]"},
        {"Dielectron_p4.Pt",       "p_{T(ee)} [GeV]"},
        {"DielectronDeltaR",       "#Delta R_{(ee)}"},
        {"Dielectron_angle.eta",   "#eta_{(ee)}"},
        {"Dielectron_angle.phi",   "#phi_{(ee)} [rad]"},
        {"Dielectron_angle.theta", "#theta_{(ee)} [rad]"},
        {"Actual_phi",             "#phi_{between(ee)} [rad]"}
    };
    

    /////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////// UNITS /////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    
    if(CrossXUnit == 1) {
		for (int i = 0; i < nFiles; i++) {
		    crossSection[i] = crossSection[i] * 1e12; // mb → fb
		    
		}
		
        std::cerr << std::endl;
		L_target = L_target * 1e-12; // mb-1 → fb-1
		std::cout << "L_target = " << L_target << " fb^{-1}" << std::endl;
        std::cerr << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// GET FILES AND TREES ////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////

    if (inputFiles.size() < 2 || inputFiles.size() != crossSection.size()) {
        std::cerr << "** ERROR: Input file and scale vectors must have the same size and at least one background file." << std::endl;
        return;
    }

    std::vector<TFile*> files(nFiles);
    std::vector<TTree*> trees(nFiles);
    std::vector<std::string> branches = {"Dielectron_p4.E"
        ,"Dielectron_p4.Px"
        , "Dielectron_p4.Py"
        , "Dielectron_p4.Pz"
        , "Dielectron_p4.Pt"
        , "DielectronMass"
        , "Dielectron_angle.eta"
        , "Dielectron_angle.phi"
        , "Dielectron_angle.theta"
        , "DielectronDeltaR"
        , "Actual_phi"};

    struct stat info;
    if (stat("PLOTS", &info) != 0) {
        system("mkdir -p PLOTS");
    }

    for (int i = 0; i < nFiles; i++) {
        files[i] = TFile::Open(inputFiles[i].c_str());
        if (!files[i]) {
            std::cerr << "** ERROR: Could not open file " << inputFiles[i] << std::endl;
            return;
        }
        trees[i] = (TTree*)files[i]->Get("Dielectrons");
        if (!trees[i]) {
            std::cerr << "** ERROR: Could not find tree 'Dielectrons' in file " << inputFiles[i] << std::endl;
            files[i]->Close();
            return;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////// SETUP HISTOGRAMS //////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    int N_Branches = 0;
    // FOR EACH BRANCH DEFINED IN THE MAP, CREATE A STACK
    for (const auto& branch : branches) {
    	N_Branches ++;
        if (ranges_x.find(branch) == ranges_x.end()) {
            std::cerr << "** ERROR: No defined range for branch " << branch << std::endl;
            continue;
        }
        

        std::vector<TH1F*> hist(nFiles);
		std::vector<int> entries;
		std::vector<double> mean, stddev;

        // Calculate bin width
        double Nbins = (ranges_x[branch].second - ranges_x[branch].first) / BinSize[branch];
        std::cerr << "Branch: " << branch << ", Nbins: " << Nbins 
                  << ", Range: [" << ranges_x[branch].first << ", " << ranges_x[branch].second 
                  << "], Bin Size: " << BinSize[branch] << std::endl;

		for (int i = 0; i < nFiles; i++) {
			hist[i] = new TH1F(Form("hist_%s_%d", branch.c_str(), i),  // name
						       branch.c_str(),                         // title
						       Nbins,                     
						       ranges_x[branch].first,                 // x-low
						       ranges_x[branch].second);               // x-high


			hist[i]->SetFillColor(colors[i % colors.size()]);
			//hist[i]->SetLineColor(colors[i % colors.size()]);
			hist[i]->SetLineColor(kWhite);
			hist[i]->SetLineWidth(3);

		}


        for (int i = 0; i < nFiles; i++) {
            trees[i]->Draw(Form("%s>>hist_%s_%d", branch.c_str(), branch.c_str(), i), "", "", N_events);
            
			entries.push_back(hist[i]->GetEntries());
			mean.push_back(hist[i]->GetMean());
			stddev.push_back(hist[i]->GetStdDev());

            hist[i]->Scale(crossSection[i] * L_target / TotalEvents[i]);

        }
        

        // Define stack
        THStack* stack = new THStack(Form("stack_%s", branch.c_str()), "");

        for (int i = 1; i < nFiles; i++) {
            stack->Add(hist[i]); // Add background first
        }
        stack->Add(hist[0]); // Add signal last

        TCanvas* c1 = new TCanvas(Form("c1_%s", branch.c_str()), "", XPixels, YPixels);
        c1->SetTopMargin(TopMargin);
        c1->SetBottomMargin(BottomMargin);
        c1->SetLeftMargin(LeftMargin);
        c1->SetRightMargin(RightMargin);
        
        gStyle->SetOptStat(0);

             
        double yMaxFinal = stack->GetMaximum() * top_margin;
        
        // Apply log scale if needed
        if (optLOGX == 1) {
            gPad->SetLogx();
        }
        if (optLOGY == 1) {
            gPad->SetLogy();
            if (range_y < 1) range_y = 1; // ensure valid positive ymin
        }
        
        // Draw frame for full control of axis range
        TH1F* frame = gPad->DrawFrame(
            ranges_x[branch].first,  // x min
            range_y,                 // y min
            ranges_x[branch].second, // x max
            yMaxFinal                // y max
        );

        frame->SetTitle("");
        frame->GetXaxis()->SetTitle(branchTitles[branch].c_str());
        frame->GetYaxis()->SetTitle(Form("Count / %g", BinSize[branch]));
        frame->GetXaxis()->SetLabelSize(TextSize);
        frame->GetXaxis()->SetTitleSize(TextSize);   
        frame->GetYaxis()->SetLabelSize(TextSize);
        frame->GetYaxis()->SetTitleSize(TextSize);
        frame->GetXaxis()->CenterTitle(true);
        frame->GetYaxis()->CenterTitle(true);
        frame->GetXaxis()->SetTitleOffset(XTitleOffset);
        frame->GetYaxis()->SetTitleOffset(YTitleOffset);

        // Optional styling to match canvas color
        if (CanvasCOLOR == 1) {
            frame->SetFillColor(kBlack); // Set the frame's background to black
            frame->GetXaxis()->SetTitleColor(kWhite);
            frame->GetYaxis()->SetTitleColor(kWhite);
            frame->GetXaxis()->SetLabelColor(kWhite);
            frame->GetYaxis()->SetLabelColor(kWhite);
            frame->GetXaxis()->SetAxisColor(kWhite);
            frame->GetYaxis()->SetAxisColor(kWhite);
            frame->SetLineColor(kWhite); // Set the frame's border line color to white
            frame->SetFillColor(kBlack); // Set the frame's background to black
        }

        // Draw the stack on top
        stack->Draw("same hist");
        
        gPad->Update();  // Important to ensure stack's axes are created
   
        if (CanvasCOLOR == 1) {
            c1->SetFillColor(kBlack);
            c1->SetFrameFillColor(kBlack);
            c1->SetFrameLineColor(kWhite);
            gStyle->SetFrameLineColor(kWhite);
            gStyle->SetHistLineColor(kWhite);
            gStyle->SetLabelColor(kWhite, "XYZ");
            gStyle->SetTitleColor(kWhite, "XYZ");
            gStyle->SetTitleTextColor(kWhite);
            gStyle->SetStatTextColor(kWhite);
            gStyle->SetTitleFillColor(kBlack);
            gStyle->SetCanvasColor(kBlack);
            gStyle->SetPadColor(kBlack);
        }
		
        gPad->SetTickx();  // Enables ticks on x-axis
		gPad->SetTicky();  // Enables ticks on y-axis
        
        if (CanvasCOLOR == 1) {
            stack->GetXaxis()->SetAxisColor(kWhite);
            stack->GetYaxis()->SetAxisColor(kWhite);
            gStyle->SetAxisColor(kWhite, "XYZ");
        }
        gStyle->SetLineWidth(3);
        gPad->Update();

        // LEGEND
        TLegend* legend = new TLegend(0.09, 0.87, 0.99, 0.99);
        legend->SetTextSize(0.029);
        legend->SetFillStyle(0);
        legend->SetBorderSize(0);

        for (int i = 0; i < nFiles; i++) {
            // First line: histogram name + cross-section
            std::string title = Form("N_{ee}: %s #mu: %.2f   Std: %.2f", approx_toKM(int(hist[i]->GetEntries())).c_str(), mean[i], stddev[i]);
            legend->AddEntry(hist[i], title.c_str(), "f");

        }

        if (CanvasCOLOR == 1) {
            legend->SetTextColor(kWhite);
        }

        legend->SetNColumns(3); // Set number of columns in the legend
        legend->Draw();
		
        // Print Leff
        double Leff = inputFiles.size() * L_target;
        std::cerr << std::endl;
        std::cout << "Leff = " << Leff << " fb^{-1}" << std::endl;
        std::cerr << std::endl;
        /* eCM and Luminosity info
        TPaveText* info = new TPaveText(0.50, 0.92, 0.90, 0.96, "NDC");
        if (CanvasCOLOR == 0) {
            info->SetFillColor(kWhite);
        }
        if (CanvasCOLOR == 1) {
            info->SetFillColor(kBlack);
            info->SetTextColor(kWhite);
        }
        info->SetTextFont(42);
        info->SetTextSize(0.03);
        info->SetBorderSize(0);

        TString text;
        Double_t Leff = inputFiles.size() * L_target;
        text.Form("#sqrt{s} = 13.6 TeV,   #int L dt = %.2f fb^{-1}", Leff);
        info->AddText(text);
        info->Draw();
        
        gStyle->SetLineWidth(2);
        gPad->RedrawAxis();
               
        stack->GetXaxis()->SetTitle(branchTitles[branch].c_str());
		stack->GetYaxis()->SetTitle("Counts");
        if (CanvasCOLOR == 1) {
            stack->GetXaxis()->SetLabelColor(kWhite);
            stack->GetYaxis()->SetLabelColor(kWhite);
            stack->GetXaxis()->SetTitleColor(kWhite);
            stack->GetYaxis()->SetTitleColor(kWhite);
        }
		*/

        gPad->Update();
        gPad->RedrawAxis();
        if (optLOGX == 0 && optLOGY == 0) {
            c1->SaveAs(Form("PLOTS/linXY/%i_%s_linXY.png", N_Branches, branch.c_str()));
            c1->Close();
            delete c1;

        }
        if (optLOGX == 0 && optLOGY == 1) {
            c1->SaveAs(Form("PLOTS/linX_logY/%i_%s_linX_logY.png", N_Branches, branch.c_str()));
            c1->Close();
            delete c1;

        }
        if (optLOGX == 1 && optLOGY == 0) {
            c1->SaveAs(Form("PLOTS/logX_linY/%i_%s_logX_linY.png", N_Branches, branch.c_str()));
            c1->Close();
            delete c1;

        }
        if (optLOGX == 1 && optLOGY == 1) {
            c1->SaveAs(Form("PLOTS/logXY/%i_%s_logXY.png", N_Branches, branch.c_str()));
            c1->Close();
            delete c1;

        }

        // BIG LEGEND
        TCanvas *c2 = new TCanvas("c2", "BigLegend", 2500, 300);
        c2->cd();

        TLegend* legend2 = new TLegend(0.01, 0.01, 0.99, 0.99);
        legend2->SetTextSize(0.25);
        legend2->SetFillStyle(0);
        legend2->SetBorderSize(0);

        for (int i = 0; i < nFiles; i++) {
            // First line: histogram name + cross-section
            std::string titleProcess = Form("%s", legendNames[i].c_str());
            legend2->AddEntry(hist[i], titleProcess.c_str(), "f");

        }

        if (CanvasCOLOR == 1) {
            legend2->SetTextColor(kWhite);
        }

        legend2->SetNColumns(3); // Set number of columns in the legend
        legend2->Draw();

        c2->SaveAs("PLOTS/BigLegend.png");
        
    }



}
