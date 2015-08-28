//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <osgEarthUtil/ObjectLocator>
#include <osgEarthUtil/LinearLineOfSight>
#include <osg/ValueObject>
#include <osg/LineWidth>
#include <osg/Depth>
#include "ChannelController.h"

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;

Define_Module(ChannelController);

ChannelController *ChannelController::instance = nullptr;

ChannelController::ChannelController()
{
    if (instance)
        throw cRuntimeError("There can be only one ChannelController instance in the network");
    instance = this;
}

ChannelController::~ChannelController()
{
    instance = nullptr;
}

ChannelController *ChannelController::getInstance()
{
    if (!instance)
        throw cRuntimeError("ChannelController::getInstance(): there is no ChannelController module in the network");
    return instance;
}

osg::ref_ptr<osg::Drawable> ChannelController::createLineBetweenPoints(osg::Vec3 start, osg::Vec3 end, float width, osg::Vec4 color) {
    osg::ref_ptr<osg::Geometry> orbitGeom = new osg::Geometry;
    osg::ref_ptr<osg::DrawArrays> drawArrayLines = new  osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP);
    osg::ref_ptr<osg::Vec3Array> vertexData = new osg::Vec3Array;

    orbitGeom->addPrimitiveSet(drawArrayLines);
    auto stateSet = orbitGeom->getOrCreateStateSet();
    stateSet->setAttributeAndModes(new osg::LineWidth(width), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    auto depth = new osg::Depth;
    depth->setWriteMask(false);
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    vertexData->push_back(start);
    vertexData->push_back(end);

    orbitGeom->setVertexArray(vertexData);
    drawArrayLines->setFirst(0);
    drawArrayLines->setCount(vertexData->size());

    osg::ref_ptr<osg::Vec4Array> colorData = new osg::Vec4Array;
    colorData->push_back(osgEarth::Color(color));
    orbitGeom->setColorArray(colorData, osg::Array::BIND_OVERALL);
    return orbitGeom;
}

int ChannelController::findSatellite(Satellite *p)
{
    for (int i = 0; i < (int)satellites.size(); i++)
        if (satellites[i] == p)
            return i;

    return -1;
}

int ChannelController::findGroundStation(GroundStation *p)
{
    for (int i = 0; i < (int)stations.size(); i++)
        if (stations[i] == p)
            return i;

    return -1;
}

void ChannelController::addLineOfSight(osg::Node *a, osg::Node *b, int type) {
    auto mapNode = osgEarth::MapNode::findMapNode(scene);
    osgEarth::Util::LinearLineOfSightNode *los = new osgEarth::Util::LinearLineOfSightNode(mapNode);
    losNodes.push_back(los);

    // not drawing the line of sight nodes' lines
    los->setGoodColor(osg::Vec4f(0, 0, 0, 0));
    los->setBadColor(osg::Vec4f(0, 0, 0, 0));

    auto stateSet = los->getOrCreateStateSet();
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    auto depth = new osg::Depth;
    depth->setWriteMask(false);
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

    los->setUserValue("type", type);

    los->setUpdateCallback(new osgEarth::Util::LineOfSightTether(a, b));
    los->setTerrainOnly(true); // so the dish model itself won't occlude

    scene->addChild(los);
}

void ChannelController::addSatellite(Satellite *p)
{
    if (findSatellite(p) == -1)
        satellites.push_back(p);
    updateConnectionGraph();
}

void ChannelController::removeSatellite(Satellite *p)
{
    int k = findSatellite(p);
    if (k != -1)
        satellites.erase(satellites.begin()+k);
}


void ChannelController::addGroundStation(GroundStation *p)
{
    if (findGroundStation(p) == -1)
        stations.push_back(p);
    updateConnectionGraph();
}

void ChannelController::removeGroundStation(GroundStation *p)
{
    int k = findGroundStation(p);
    if (k != -1)
        stations.erase(stations.begin()+k);
}

void ChannelController::initialize(int stage)
{
    switch (stage) {
    case 0:
        satToSatColor = par("satToSatColor").stringValue();
        satToSatWidth = par("satToSatWidth").doubleValue();
        satToGroundColor = par("satToGroundColor").stringValue();
        satToGroundWidth = par("satToGroundWidth").doubleValue();
        break;
    case 1:
        connections = new osg::Geode();
        scene = OsgEarthScene::getInstance()->getScene()->asGroup();
        scene->addChild(connections);
        break;
    case 2:
        for (int i = 0; i < (int)satellites.size(); ++i) {
            for (int j = 0; j < (int)stations.size(); ++j)
                addLineOfSight(satellites[i]->getLocatorNode(), stations[j]->getLocatorNode(), 0);

            for (int j = i+1; j < (int)satellites.size(); ++j)
                addLineOfSight(satellites[i]->getLocatorNode(), satellites[j]->getLocatorNode(), 1);
        }
        break;
    }
}

void ChannelController::updateConnectionGraph()
{
    connections->removeDrawables(0, connections->getNumDrawables());

    int ns2s = 0;
    int ns2g = 0;

    for (auto n : losNodes) {
        if (n->getHasLOS()) {
            auto start = n->getStartWorld();
            auto end = n->getEndWorld();

            int type;
            n->getUserValue("type", type);
            switch (type) {
            case 0:
                ++ns2g;
                if (!satToGroundColor.empty())
                    connections->addDrawable(createLineBetweenPoints(start, end, satToGroundWidth, osgEarth::Color(satToGroundColor)));
                break;
            case 1:
                ++ ns2s;
                if (!satToSatColor.empty())
                    connections->addDrawable(createLineBetweenPoints(start, end, satToSatWidth, osgEarth::Color(satToSatColor)));
                break;
            }
        }
    }

    EV << "Active connections: " << ns2s << " sat-to-sat and " << ns2g << " sat-to-ground \n";
}

void ChannelController::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not process messages");
}