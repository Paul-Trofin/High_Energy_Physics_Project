/* Run like this:
 * 
root -l "EventDisplay.C(\"$HOME/delphes/cards/delphes_card_ATLAS.tcl\", \"$HOME/Simulations/DelphesSimulations/pp/ATLAS/1M_events/processes/qq_Z_ee/qq_Z_ee.root\")"
 */

#ifdef __CLING__
R__LOAD_LIBRARY(libEve)
R__LOAD_LIBRARY(libDelphesDisplay)
#include "display/DelphesEventDisplay.h"
#include "display/Delphes3DGeometry.h"
#endif

void EventDisplay(const char *configfile = "delphes_card_CMS.tcl",
                  const char *datafile = "delphes_output.root",
                  const char *ParticlePropagator = "ParticlePropagator",
                  const char *TrackingEfficiency = "ElectronTrackingEfficiency, ChargedHadronTrackingEfficiency",
                  const char *ElectronEfficiency = "ElectronEfficiency",
                  const char *Calorimeters = "ECal",
                  bool displayGeometryOnly = false)
{
  // load the libraries
  gSystem->Load("libGeom");
  gSystem->Load("libGuiHtml");
  gSystem->Load("libDelphesDisplay");

  if(displayGeometryOnly)
  {
    // create the detector representation without transparency
    Delphes3DGeometry det3D_geom(new TGeoManager("delphes", "Delphes geometry"), false);
    det3D_geom.readFile(configfile, ParticlePropagator, TrackingEfficiency, ElectronEfficiency, Calorimeters);

    // display
    det3D_geom.getDetector()->Draw("ogl");
  } 
  else
  {
    // create the detector representation
    Delphes3DGeometry det3D(new TGeoManager("delphes", "Delphes geometry"), true);
    det3D.readFile(configfile, ParticlePropagator, TrackingEfficiency, ElectronEfficiency, Calorimeters);

    // create the application
    DelphesEventDisplay *display = new DelphesEventDisplay(configfile, datafile, det3D);
  }
}
