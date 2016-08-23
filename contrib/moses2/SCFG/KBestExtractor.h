/*
 * KBestExtractor.h
 *
 *  Created on: 2 Aug 2016
 *      Author: hieu
 */
#pragma once
#include <vector>
#include <sstream>
#include <boost/unordered_map.hpp>
#include "../ArcLists.h"
#include "TargetPhraseImpl.h"

namespace Moses2
{
class Scores;

namespace SCFG
{
class Manager;
class Hypothesis;
class NBestColl;
class NBests;

/////////////////////////////////////////////////////////////
class NBest
{
public:
	const ArcList *arcList;
	size_t ind;

	typedef std::pair<const NBests*, size_t> Child; // key to another NBest
	typedef std::vector<Child> Children;
	Children children;

	NBest(const SCFG::Manager &mgr,
			const NBestColl &nbestColl,
			const ArcList &varcList,
			size_t vind);

	const Scores &GetScores() const
	{ return *m_scores; }

    void OutputToStream(
    		const SCFG::Manager &mgr,
			std::stringstream &strm,
			const NBestColl &nbestColl) const;

protected:
	Scores *m_scores;

	const SCFG::Hypothesis &GetHypo() const;
};

/////////////////////////////////////////////////////////////
class NBests : public std::vector<NBest*>
{
public:
	virtual ~NBests();
};

/////////////////////////////////////////////////////////////
class NBestColl
{
public:
	virtual ~NBestColl();

	void Add(const SCFG::Manager &mgr, const ArcList &arcList);
	const NBests &GetNBests(const ArcList &arcList) const;

protected:
	typedef boost::unordered_map<const ArcList*, NBests*> Coll;
	Coll m_candidates;

	NBests &GetOrCreateNBests(const ArcList &arcList);
};

/////////////////////////////////////////////////////////////
class KBestExtractor
{
public:
  KBestExtractor(const SCFG::Manager &mgr);
  virtual ~KBestExtractor();

  void OutputToStream(std::stringstream &strm);
protected:
  const SCFG::Manager &m_mgr;
  NBestColl m_nbestColl;
};

}
} /* namespace Moses2 */