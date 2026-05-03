---
# **Z Boson Mass Reconconstruction at L3 vs. ATLAS Using Fast Detector Simulations**

---
## *Author: Paul Trofin*
---

### *Python3* script used to generate data with *Delphes3*, with *Pythia8* included. Generates all files necessary for HEP MC data production.
### This script was mainly used for producing data in which at least one electron and one positron are produced at a primary vertex. 
---

### Required programs:
- Delphes
- Pythia
- ROOT

> Note: The executable "./DelphesPyhtia8" is required. It is an optional executable that can be installed within Delphes3.

> Note: Delphes3, Pyhtia8, and ROOT have to be properly installed and exported to PATH.
---

## Let's run the first (default) example:
- Clone the GitHub Repository
```bash
git clone https://github.com/Paul-Trofin/TPaul_Z_Mass_JHEP
```
- Run the *python3* script "generate_delphes_macros.py"
```bash
python3 generate_delphes_macros.py
cd qq_Z_ee
```
- Open the command file "qq_Z_ee.cmnd":
  - Here you will find the command line to run from a terminal inside the Delphes Installation Directory.
  - Just copy and paste the command line.
  - The executable "./DelphesPyhtia8" should start running, and counting events normally.
  - After the 10k events were generated, two additional files are created:
    - "qq_Z_ee.log" (Pythia ".log" file from which the cross sections for the hard process were extracted).
    - ""qq_Z_ee.root" (".root" file where the events are stored, in Delphes format).
- Open the "get_variables.C" file:
  - Just copy and paste the command line, and run in a terminal in the same directory.
```bash
root -l get_variables.C'("qq_Z_ee.root")'
```
  - Another file is created "variables.root" which contains all the information needed for extracting the four-momenta of the electron-positron pair, necessary for calculating the kinematic variables used (especially the invariant mass of the Z boson).

---
## You can change the options as you like, to suit your desired HEP MC data production.
---
## Python3 Script "generate_delphes_macros.py" Overview:
- OPTIONS (used for ".cmnd" files):
  - Input the beams id, center of mass energy (required)
  - Hard process (required)
  - ### Optional settings:
    - Include (append) whatever valid Pythia options you like.
    - In this study, no MPI, ISR, FSR were used, while hadronization is.
- End of the options options part. You can change manually from here
- BANNER
- Write the ".cmnd" and transform to absolute paths for easier use.
- ROOT ".C" Script for Analysis:
  - LOAD Delphes and ROOT libraries
  - MAIN CODE (takes as input a ".root" file in Delphes format):
    - Declare proper ROOT arrays to store electrons (particles of interest), Jets, Missing Tansverse Energy
    - Store the four-momenta of the particles in ROOT Trees.
    - Each branch has sub-branches to store information.
    - LOOP OVER EVENTS:
      - Store Electrons:
        - Nr. of e-, e- four-momentum, e- angles.
      - Store Positrons:
        - Nr. of e+, e+ four-momentum, e+ angles. 
      - Dielectrons (e-e+ pairs):
        - Nr. of dielectrons
        - Dielectron DeltaR
        - Dielectron Mass (inferring the mass of the Z)
        - Dielectron four-momentum
        - Dielectron angles
        - Actual phi (geometric angle between e- and e+)
      - Jets:
        - Jet multiplicity, four-momentum, angles

---
## You may use this script as you like to produce your desired HEP MC data.
---


## Inside the "Analysis" folder you will find:
- cut_variables.C (used to refine the data)
- stack_hist.C (used to stack histograms on top of each other for analysis)
- plot_DielectronMass.C (used to plot the mass of the e-e+ pairs)
- EventDisplay.C (used to visualize via Delphes GUI each event)






