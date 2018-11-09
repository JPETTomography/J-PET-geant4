#ifndef DetectorSD_h
#define DetectorSD_h 1

#include "G4VSensitiveDetector.hh"
#include "DetectorHit.hh"

class DetectorSD : public G4VSensitiveDetector
{
    public:
        DetectorSD(G4String name);
        virtual ~DetectorSD();

        virtual void Initialize(G4HCofThisEvent* HCE);

    private:
        const G4double timeIntervals = 0.08; // 80 ps
/*        const G4int totScinNum = 193;                //original
        G4int previousHitHistory[193];                 //original
        G4double previousHitTimeHistory[193];          //original
*/
        const G4int totScinNum = 505;                //original
        G4int previousHitHistory[505];                 //original
        G4double previousHitTimeHistory[505];          //original
        //const G4int totScinNum = 7; 
        //G4int previousHitHistory[7];
        //G4double previousHitTimeHistory[7];

        DetectorHitsCollection *fDetectorCollection;
        //G4int lastHitID;
        //G4double previousTime;

    protected:
         virtual G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);


};


#endif
