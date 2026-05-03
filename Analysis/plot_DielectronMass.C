// T
void plot_DielectronMass()
{
    // Open ROOT file and tree
    TFile *file = TFile::Open("all_cut.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open variables_all_cut.root\n";
        return;
    }

    TTree *tree = (TTree*) file->Get("Dielectrons");
    if (!tree) {
        std::cerr << "Error: Dielectrons tree not found!\n";
        return;
    }

    // Style settings
    gStyle->SetOptStat(0);       // disable stats box
    gStyle->SetOptFit(1);        // enable fit parameters box

    gStyle->SetStatFontSize(0.04);
    
    // Beautify Canvas
    
    gStyle->SetCanvasColor(kBlack);
    gStyle->SetPadColor(kBlack);
    gStyle->SetFrameFillColor(kBlack);
    gStyle->SetFrameLineColor(kWhite);
    gStyle->SetTitleTextColor(kWhite);
    gStyle->SetLabelColor(kWhite, "XYZ");
    gStyle->SetTitleColor(kWhite, "XYZ");
    gStyle->SetAxisColor(kWhite, "XYZ");
    gStyle->SetStatTextColor(kWhite);
    

    // Canvas
    TCanvas *c1 = new TCanvas("c1", "Dielectron Mass", 1920, 1080);
    c1->SetTickx();
    c1->SetTicky();
    c1->SetGridy();
    
    // for black canvas
    c1->SetFillColor(kBlack);
    c1->SetFrameFillColor(kBlack);
    c1->SetFrameLineColor(kWhite);
    

    // Histogram
    tree->Draw("DielectronMass>>hDielectronMass(500,66,116)", "", "hist");
    TH1F *hist = (TH1F*) gDirectory->Get("hDielectronMass");
    hist->SetStats(0);  // Completely hide stats box after all fits
    if (!hist) {
        std::cerr << "Error: Histogram not generated!\n";
        return;
    }

    hist->SetFillColor(kGray+1);
    hist->SetLineColor(kGray+2);
    hist->SetLineWidth(2);
    hist->SetTitle("");
    hist->GetXaxis()->SetTitle("M_{Z} (GeV)");
    hist->GetYaxis()->SetTitle("Counts / 0.1 GeV");
    
	// for black canvas
    hist->GetXaxis()->SetTitleColor(kWhite);
    hist->GetXaxis()->SetLabelColor(kWhite);
    hist->GetXaxis()->SetAxisColor(kWhite);
    hist->GetYaxis()->SetTitleColor(kWhite);
    hist->GetYaxis()->SetLabelColor(kWhite);
    hist->GetYaxis()->SetAxisColor(kWhite);
    

    int maxBin = hist->GetMaximumBin();          // Get bin number with maximum content
    double maxValue = hist->GetBinContent(maxBin);  // Get the bin content (height)
    double maxX = hist->GetBinCenter(maxBin);       // Get the x-axis value at that bin

	// Define fit function, fit histogram, and draw
	const int DeeperCrimsonRed = TColor::GetColor(150, 20, 40);
	const int DeepNeonGreen    = TColor::GetColor(0, 180, 90);
    const int DeepElectricBlue = TColor::GetColor(20, 90, 160);
	
	TF1 *gausFit1 = new TF1("gausFit1", "gaus", 66, 116);  // Broad initial range
	gausFit1->SetLineColor(DeepNeonGreen);
	gausFit1->SetLineWidth(5);  // Thicker line
	gausFit1->SetNpx(1000);     // Smooth curve

	hist->Fit(gausFit1, "R");   // First fit in broad range
	hist->Draw("hist");         // Draw histogram after fit
	gausFit1->Draw("same");     // Overlay first fit

	// Extract mean and sigma from first fit
    double constant1 = gausFit1->GetParameter(0);
	double mean1 = gausFit1->GetParameter(1);
	double sigma1 = gausFit1->GetParameter(2);

	// Define refined fit range around mean ± 2σ
	double fitLow2 = mean1 - 2 * sigma1;
	double fitHigh2 = mean1 + 2 * sigma1;

	// Second Gaussian fit within narrower refined range
	TF1 *gausFit2 = new TF1("gausFit2", "gaus", fitLow2, fitHigh2);
	gausFit2->SetLineColor(DeeperCrimsonRed);
	gausFit2->SetLineWidth(5);  // Thicker line
	gausFit2->SetNpx(1000);
	hist->Fit(gausFit2, "R+");  // "+" keeps first fit on plot
	gausFit2->Draw("same");

    // Extract mean and sigma from second fit
    double constant2 = gausFit2->GetParameter(0);
    double mean2 = gausFit2->GetParameter(1);
    double sigma2 = gausFit2->GetParameter(2);

    /*// Define refined fit range around mean ± 2sigma
	double fitLow3 = mean2 - 2 * sigma2;
	double fitHigh3 = mean2 + 2 * sigma2;
    // Third Gaussian fit
    TF1 *gausFit3 = new TF1("gausFit3", "gaus", fitLow3, fitHigh3);
    gausFit3->SetParameters(constant2, mean2, sigma2);
    gausFit3->SetLineColor(DeepElectricBlue);
    gausFit3->SetLineWidth(5);  // Thicker line
    gausFit3->SetNpx(1000);
    hist->Fit(gausFit3, "R+");  // "+" keeps first fit on plot
    gausFit3->Draw("same");*/

	// Update pad to ensure stats box appears after all drawing
	gPad->Update();
    // -- Construct legend with custom info
    TLegend *leg = new TLegend(0.60, 0.65, 0.83, 0.80);
    //leg->SetTextColor(kWhite); black canvas
    leg->SetTextSize(0.035);
    leg->SetTextColor(kWhite);
    leg->SetFillStyle(0);  // transparent
    //leg->SetLineColor(0);  // no border
    leg->SetBorderSize(0);

    // Histogram stats
    leg->AddEntry(hist, Form("Entries = %.0f", hist->GetEntries()), "");
    leg->AddEntry((TObject*)0, Form("Mean = %.3f", hist->GetMean()), "");
    leg->AddEntry((TObject*)0, Form("Std Dev = %.3f", hist->GetStdDev()), "");

    // First fit
    leg->AddEntry(gausFit1, Form("Fit 1: #mu = %.3f, Std = %.3f", 
                                gausFit1->GetParameter(1), gausFit1->GetParameter(2)), "l");

    // Second fit
    leg->AddEntry(gausFit2, Form("Fit 2: #mu = %.3f, Std = %.3f", 
                                gausFit2->GetParameter(1), gausFit2->GetParameter(2)), "l");

    /*leg->AddEntry(gausFit3, Form("Fit 3: #mu = %.3f #pm %.3f", 
                                gausFit3->GetParameter(1), gausFit3->GetParError(1)), "l");*/

    // Draw on canvas
    leg->Draw();

    // Save canvas
    gPad->Update();            // ensure everything is drawn
    c1->SaveAs("DielectronMass_Cut.png");

    file->Close();
    delete c1;
}
C
