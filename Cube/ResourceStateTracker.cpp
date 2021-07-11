#include "ResourceStateTracker.h"

ResourceStateTracker::ResourceStateTracker() {};

ResourceStateTracker::~ResourceStateTracker() {}

void ResourceStateTracker::reset()
{
   m_pendingResourceBarriers.clear();
   m_resourceBarriers.clear();
   m_finalResourceState.clear();
}

