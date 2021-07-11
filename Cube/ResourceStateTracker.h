#pragma once


/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

 /**
  *  @file ResourceStateTracker.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Tracks the known state of a (sub)resource within a command list.
  *
  *  The ResourceStateTracker tracks the known state of a (sub)resource within a command list.
  *  It is often difficult (or impossible) to know the current state of a (sub)resource if
  *  it is being used in multiple command lists. For example when doing shadow mapping,
  *  a depth buffer resource is being used as a depth-stencil view in a command list
  *  that is generating the shadow map for a light source, but needs to be used as
  *  a shader-resource view in a command list that is performing shadow mapping. If
  *  the command lists are being generated in separate threads, the exact state of the
  *  resource can't be guaranteed at the moment it is used in the command list.
  *  The ResourceStateTracker class is intended to be used within a command list
  *  to track the state of the resource as it is known within that command list.
  *
  *  @see https://youtu.be/nmB2XMasz2o
  *  @see https://msdn.microsoft.com/en-us/library/dn899226(v=vs.85).aspx#implicit_state_transitions
  */

#include <d3d12.h>
#include <wrl/client.h>

#include <unordered_map>
#include <vector>


class CommandList;
class Resource;

class ResourceStateTracker
{
public:
   ResourceStateTracker();
   virtual ~ResourceStateTracker();

   void reset();

private:
   //using ResourceBarriers = std::vector< D3D12_RESOURCE_BARRIER>;
   
   struct ResourceState
   {
      explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON) :
         State(state)
      {}

      void setSubresourceState(UINT subresource, D3D12_RESOURCE_STATES state)
      {
         if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
         {
            State = state;
            SubresourceState[subresource] = state;
         }
         else
         {
            SubresourceState[subresource] = state;
         }
      }

      D3D12_RESOURCE_STATES getSubresourceState(UINT subresource) const
      {
         D3D12_RESOURCE_STATES state = State;
         const auto iter = SubresourceState.find(subresource);
         if (iter != SubresourceState.end())
         {
            state = iter->second;
         }
         return state;
      }

      D3D12_RESOURCE_STATES State;
      std::unordered_map<UINT, D3D12_RESOURCE_STATES> SubresourceState;
   };

   std::vector< D3D12_RESOURCE_BARRIER> m_pendingResourceBarriers;
   std::vector< D3D12_RESOURCE_BARRIER> m_resourceBarriers;
   std::unordered_map<ID3D12Resource*, ResourceState> m_finalResourceState;
};

