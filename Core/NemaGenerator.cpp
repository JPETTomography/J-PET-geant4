/**
 *  @copyright Copyright 2021 The J-PET Monte Carlo Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  @file NemaGenerator.cpp
 */

#include <Randomize.hh>
#include <globals.hh>

#include "PrimaryGenerator.h"

NemaPoint::NemaPoint()
{
  G4ThreeVector temp(0,0,0);
  position = temp;
  oPslifetime = MaterialParameters::fTauBulk;
  is3GAllowed = false;
  G4ThreeVector temp2(0.1,0.1,0.1);
  sizeOfPoint = temp2;
  orientationOfPoint = temp;
  shapeOfPointInY = temp;
}

void NemaGenerator::AddPoint(G4int pointID)
{
  auto search = fIDPointsConnection.find(pointID);
  if (search == fIDPointsConnection.end()) {
    G4int genPointSize = fGeneratedPoints.size();
    NemaPoint newPoint;
    fGeneratedPoints.push_back(newPoint);
    fIDPointsConnection.insert(std::make_pair(pointID, genPointSize));
  }
}

void NemaGenerator::AddPointWeight(G4int pointID, G4int weight)
{
  unsigned removedElements = 0;
  if (weight == 0) {
    for (unsigned i=0; i<fWeightPositions.size(); i++) {
      if (fWeightPositions.at(i-removedElements) == pointID) {
        fWeightPositions.erase(fWeightPositions.begin()+i-removedElements);
        removedElements++;
      }
    }
  } else {
    G4int count = 0;
    for (unsigned i=0; i<fWeightPositions.size(); i++) {
      if (fWeightPositions.at(i) == pointID)
        count++;
    }
    if (weight >= count) {
      for (G4int i=0; i<weight-count; i++)
        fWeightPositions.push_back(pointID);
    } else {
      G4int nmbOfPointToErase = count - weight;
      for (unsigned i=0; i<fWeightPositions.size(); i++) {
        if (fWeightPositions.at(i-removedElements) == pointID) {
          if ((int)removedElements < nmbOfPointToErase) {
            fWeightPositions.erase(fWeightPositions.begin()+i-removedElements);
            removedElements++;
          }
        }
      }
    }
  }
}

void NemaGenerator::SetOnePointOnly(G4int pointID)
{
  NemaPoint tempPoint = GetPoint(pointID);
  Clear();
  fGeneratedPoints.push_back(tempPoint);
  fWeightPositions.push_back(pointID);
  fIDPointsConnection.insert(std::make_pair(pointID, 0));
}

bool NemaGenerator::DoesPointExistAlready(G4int pointID) const
{
  auto search = fIDPointsConnection.find(pointID);
  if (search == fIDPointsConnection.end()) {
    return false;
  } else {
    return true;
  }
}

NemaPoint NemaGenerator::GetPoint(G4int pointID) const
{
  auto search = fIDPointsConnection.find(pointID);
  if (search != fIDPointsConnection.end()) {
    return fGeneratedPoints.at(fIDPointsConnection.at(pointID));
  } else {
    NemaPoint newPoint;
    return newPoint;
  }
}

NemaPoint NemaGenerator::GetRandomPoint() const
{
  if (fGeneratedPoints.size() == 1)
    return fGeneratedPoints.at(0);
  
  G4double random = G4UniformRand();
  unsigned indexOfPoint = (int)(random * fWeightPositions.size());
  G4int pointID = fWeightPositions.at(indexOfPoint);
  return GetPoint(pointID);
}

void NemaGenerator::GenerateElipseYNorm(G4int pointID)
{
  Int_t pointNumber = 1000;
  Double_t thetaStep = M_PI/(2*pointNumber);
  Double_t maxVal = 1;
  G4int pointVectorID = fIDPointsConnection.at(pointID);
  G4double yMax = fGeneratedPoints.at(pointVectorID).shapeOfPointInY.getX();
  G4double zMax = fGeneratedPoints.at(pointVectorID).sizeOfPoint.getZ();
  if (fabs(yMax) > fabs(zMax)) {
    maxVal = fabs(yMax);
  }
  else {
    maxVal = fabs(zMax);
  }
  Double_t arg = 0, val = 0;
  TGraph* newGraph = new TGraph(pointNumber);
  for (Int_t i=0; i<pointNumber; i++) {
    arg = thetaStep*i;
    val = sqrt(pow(yMax*sin(thetaStep*i),2) + pow(zMax*cos(thetaStep*i),2))/maxVal;
    newGraph -> SetPoint(i, arg, val);
  }
  fGeneratedPoints.at(pointVectorID).elipseYNorm = new TF1("elipseY", "pol8", 0, M_PI/2);
  newGraph -> Fit(fGeneratedPoints.at(pointVectorID).elipseYNorm, "Q");
  delete newGraph;
}

G4ThreeVector NemaGenerator::GetPointShapedInY(G4ThreeVector vtxPosition, NemaPoint nemaPoint)
{
  G4ThreeVector newVtxPosition = vtxPosition;
  if (nemaPoint.shapeOfPointInY.getX() != 0) {
    G4double previousY = newVtxPosition.getY();
    G4double directionY = nemaPoint.shapeOfPointInY.getX();
    G4double cutOffY = 1-nemaPoint.shapeOfPointInY.getZ();
    while (true) {
      G4double randNum = G4UniformRand();
      G4double randNum2 = cutOffY*(M_PI * G4UniformRand() - M_PI/2);
      if (randNum <= nemaPoint.elipseYNorm -> Eval(fabs(randNum2))) {
        G4double yP = directionY*cos(randNum2);
        G4double zP = nemaPoint.sizeOfPoint.getZ()*sin(randNum2);
        G4double signZ = zP/fabs(zP);
        G4double theta = acos(yP/fabs(directionY));
        yP = fabs(directionY)*cos(theta);
        zP = signZ*nemaPoint.sizeOfPoint.getZ()*sin(theta);
        yP = yP + previousY*cos(theta);
        zP = zP + signZ*previousY*sin(theta);
        newVtxPosition.setY(yP);
        newVtxPosition.setZ(zP);
        break;
      }
    }
  }
  return newVtxPosition;
}

G4ThreeVector NemaGenerator::GetRotatedPoint(G4ThreeVector vtxPosition, NemaPoint nemaPoint)
{
  G4ThreeVector newVtxPosition = vtxPosition;
  if (nemaPoint.orientationOfPoint.mag() > 0) {
    G4double theta = nemaPoint.orientationOfPoint.getX() * M_PI/180.;
    G4double phi = nemaPoint.orientationOfPoint.getY() * M_PI/180.;
    if (theta != 0. || phi != 0.) {
      TVector3 normal(cos(theta)*sin(phi), sin(theta)*sin(phi), cos(phi));
      normal = normal.Unit();
      TVector3 z(0.,0.,1.);
      TVector3 rotAxis = z.Cross(normal);
      double angle = normal.Angle(z); // radians
      TRotation rot;
      rot.Rotate(angle, rotAxis);
      TVector3 tempVector(newVtxPosition.getX(), newVtxPosition.getY(), newVtxPosition.getZ());
      tempVector = rot*tempVector;
      newVtxPosition.setX(tempVector.X());
      newVtxPosition.setY(tempVector.Y());
      newVtxPosition.setZ(tempVector.Z());
    }
  }
  return newVtxPosition;
}
