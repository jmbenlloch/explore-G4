#include "detector_construction.hh"

#include "nain4.hh"

#include <G4Box.hh>
#include <G4Cons.hh>
#include <G4Trd.hh>

#include <G4SystemOfUnits.hh>

using nain4::material;
using nain4::logical;
using nain4::place;


G4VPhysicalVolume* detector_construction::Construct() {

  // ----- Materials --------------------------------------------------------------
  auto air    = material("G4_AIR");
  auto water  = material("G4_WATER");
  auto tissue = material("G4_A-150_TISSUE");
  auto bone   = material("G4_BONE_COMPACT_ICRU");

  // ----- Dimensions -------------------------------------------------------------
  // Size of the detector
  auto length_xy = 20 * cm;
  auto length_z  = 30 * cm;

  // Envelope: G4Box requires half-lengths
  auto e_xy = 0.5 * length_xy;
  auto e_z  = 0.5 * length_z;

  // World volume needs a margin around everything inside it
  auto w_xy = 1.2 * e_xy;
  auto w_z  = 1.2 * e_z;

  // Trapezoid ---------------------------------------------------------------------
  auto t_dxa = 12 * cm / 2, t_dxb = 12 * cm / 2;
  auto t_dya = 10 * cm / 2, t_dyb = 16 * cm / 2;
  auto t_dz  =  6 * cm / 2;

  // Cone --------------------------------------------------------------------------
  auto c_rmin_a  = 0 * cm,   c_rmax_a = 2 * cm;
  auto c_rmin_b  = 0 * cm,   c_rmax_b = 4 * cm;
  auto c_hz      = 3 * cm;
  auto c_phi_min = 0 * deg,  c_phi_max = 360 * deg;

  // ----- Create the components of the detector ------------------------------------
  auto world     = logical<G4Box> ("World"        , air   , w_xy, w_xy, w_z);
  auto envelope  = logical<G4Box> ("Envelope"     , water , e_xy, e_xy, e_z);
  auto trapezoid = logical<G4Trd> ("BoneTrapezoid", bone  , t_dxa, t_dxb, t_dya, t_dyb, t_dz);
  auto cone      = logical<G4Cons>("TissueCone"   , tissue, c_rmin_a, c_rmax_a, c_rmin_b, c_rmax_b, c_hz, c_phi_min, c_phi_max);

  this->scoring_volume = trapezoid;

  // ----- Combine the components ---------------------------------------------------
  place(trapezoid).in(envelope).at(0, -1*cm, 7*cm).now();
  place(cone     ).in(envelope).at(0,  2*cm,-7*cm).now();
  place(envelope ).in(world   )                   .now();

  return place(world).now();
}
