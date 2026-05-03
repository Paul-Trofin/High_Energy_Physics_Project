#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <iostream>

void cut_variables() {
    // Open input file
    TFile *inFile = TFile::Open("final.root"); // Your final root file
    if (!inFile || inFile->IsZombie()) {
        std::cerr << "Error opening file final.root" << std::endl;
        return;
    }

    // Get the Dielectrons tree
    TTree *inTree = (TTree*)inFile->Get("Dielectrons");
    if (!inTree) {
        std::cerr << "Dielectrons tree not found!" << std::endl;
        return;
    }

    // Variables to read branches
    Int_t NumberOfElectronPairs;
    Double_t DielectronMass;
    Double_t DielectronDeltaR;
    Double_t Actual_phi;

    // Structs in branches
    struct {
        Double_t E, Px, Py, Pz, Pt;
    } Dielectron_p4;

    struct {
        Double_t eta, phi, theta;
    } Dielectron_angle;

    // Set branch addresses
    inTree->SetBranchAddress("NumberOfElectronPairs", &NumberOfElectronPairs);
    inTree->SetBranchAddress("DielectronMass", &DielectronMass);
    inTree->SetBranchAddress("DielectronDeltaR", &DielectronDeltaR);
    inTree->SetBranchAddress("Actual_phi", &Actual_phi);
    inTree->SetBranchAddress("Dielectron_p4", &Dielectron_p4);
    inTree->SetBranchAddress("Dielectron_angle", &Dielectron_angle);

    // Create output file and tree
    TFile *outFile = new TFile("selected.root", "RECREATE");
    TTree *outTree = inTree->CloneTree(0);  // Empty clone with same branches

    Long64_t nEntries = inTree->GetEntries();

    for (Long64_t i = 0; i < nEntries; ++i) {
        inTree->GetEntry(i);

        // Compute Angle_between - angle between the two electrons is given by theta from Dielectron_angle
        Double_t Angle_between = Dielectron_angle.theta;

        // Apply cuts
        if ((DielectronMass > 66 && DielectronMass < 116) &&
            (std::abs(Dielectron_angle.eta) > 5) &&
            (std::abs(Angle_between) > (TMath::Pi() - 0.1)) &&
            (std::abs(Angle_between) < (TMath::Pi() + 0.1)) &&
            (std::abs(Dielectron_p4.Pt) < 1)) {

            outTree->Fill();  // Save this entry
        }
    }

    // Write and close output file
    outTree->Write();
    outFile->Close();
    inFile->Close();

    std::cout << "Selection done. Output saved to selected.root" << std::endl;
}

