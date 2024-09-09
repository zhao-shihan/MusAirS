#include "MusAirS/Action/PrimaryGeneratorAction.h++"
#include "MusAirS/Action/TrackingAction.h++"
#include "MusAirS/Analysis.h++"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"

namespace MusAirS::inline Action {

PrimaryGeneratorAction::PrimaryGeneratorAction() :
    PassiveSingleton{},
    G4VUserPrimaryGeneratorAction{},
    fAvailableGenerator{},
    fGenerator{&fAvailableGenerator.pcr},
    fSavePrimaryVertexData{false},
    fPrimaryVertexData{},
    fActionMessengerRegister{this},
    fAnalysisMessengerRegister{this} {}

auto PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) -> void {
    fGenerator->GeneratePrimaryVertex(event);
    if (fSavePrimaryVertexData) { UpdatePrimaryVertexData(*event); }
}

auto PrimaryGeneratorAction::UpdatePrimaryVertexData(const G4Event& event) -> void {
    fPrimaryVertexData.clear();
    fPrimaryVertexData.reserve(event.GetNumberOfPrimaryVertex());
    for (const auto* pv{event.GetPrimaryVertex()}; pv; pv = pv->GetNext()) {
        for (const auto* pp{pv->GetPrimary()}; pp; pp = pp->GetNext()) {
            auto& v{fPrimaryVertexData.emplace_back()};
            Get<"EvtID">(v) = event.GetEventID();
            Get<"PDGID">(v) = pp->GetPDGcode();
            Get<"t0">(v) = pv->GetT0();
            Get<"x0">(v) = pv->GetPosition();
            Get<"Ek0">(v) = pp->GetKineticEnergy();
            Get<"p0">(v) = pp->GetMomentum();
            Get<"Weight">(v) = pv->GetWeight();
        }
    }
    Analysis::Instance().SubmitPrimaryVertexData(fPrimaryVertexData);
}

} // namespace MusAirS::inline Action
