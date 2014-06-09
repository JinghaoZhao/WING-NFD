/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014,  Regents of the University of California,
 *                      Arizona Board of Regents,
 *                      Colorado State University,
 *                      University Pierre & Marie Curie, Sorbonne University,
 *                      Washington University in St. Louis,
 *                      Beijing Institute of Technology,
 *                      The University of Memphis
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rib-entry.hpp"

#include <ndn-cxx/management/nfd-control-command.hpp>

namespace nfd {
namespace rib {

RibEntry::FaceList::iterator
RibEntry::findFace(const FaceEntry& face)
{
  return std::find_if(begin(), end(), bind(&compareFaceIdAndOrigin, _1, face));
}

bool
RibEntry::insertFace(const FaceEntry& entry)
{
  iterator it = findFace(entry);

  if (it == end())
    {
      m_faces.push_back(entry);
      return true;
    }
  else
    {
      return false;
    }
}

bool
RibEntry::eraseFace(const FaceEntry& face)
{
  RibEntry::iterator it = std::find_if(begin(), end(), bind(&compareFaceIdAndOrigin, _1, face));
  if (it != m_faces.end())
    {
      m_faces.erase(it);
      return true;
    }
  else
    {
      return false;
    }
}

bool
RibEntry::hasFaceId(const uint64_t faceId) const
{
  RibEntry::const_iterator it = std::find_if(cbegin(), cend(), bind(&compareFaceId, _1, faceId));

  return (it != cend());
}

void
RibEntry::addChild(shared_ptr<RibEntry> child)
{
  BOOST_ASSERT(!static_cast<bool>(child->getParent()));
  child->setParent(this->shared_from_this());
  m_children.push_back(child);
}

void
RibEntry::removeChild(shared_ptr<RibEntry> child)
{
  BOOST_ASSERT(child->getParent().get() == this);
  child->setParent(shared_ptr<RibEntry>());
  m_children.remove(child);
}

RibEntry::FaceList::iterator
RibEntry::eraseFace(FaceList::iterator face)
{
  return m_faces.erase(face);
}

std::ostream&
operator<<(std::ostream& os, const FaceEntry& entry)
{
  os << "FaceEntry("
     << " faceid: " << entry.faceId
     << " origin: " << entry.origin
     << " cost: " << entry.cost
     << " flags: " << entry.flags
     << " expires in: " << (entry.expires - time::steady_clock::now())
     << ")";

  return os;
}

std::ostream&
operator<<(std::ostream& os, const RibEntry& entry)
{
  os << "RibEntry{\n";
  os << "\tName:  " << entry.getName() << "\n";

  for (RibEntry::FaceList::const_iterator faceIt = entry.cbegin(); faceIt != entry.cend(); ++faceIt)
    {
      os << "\t" << (*faceIt) << "\n";
    }

  os << "}";

  return os;
}

} // namespace rib
} // namespace nfd
