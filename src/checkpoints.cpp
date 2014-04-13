// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2013-2014 Dogecoin Developers
// Copyright (c) 2013-2014 Team Mooncoin
// Copyright (c) 2014 UltimateCoin Developers and Research Group
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
	// Adding two extra checkpoints past Genesis, 10 & 100
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (    0, hashGenesisBlockOfficial)
       	(    1, uint256("0xce9f90bee2ac2a641db8f0c0fd7f1475a6f07f90da4eadb70a6f8371393767c2"))
		(    2, uint256("0x9170a46e5ad259b58ca1d250f8b24c457325dbab97a19dd6b95c3e269632e691"))
		(    3, uint256("0x0f9a036d14365e492bcd13a7e16e9315898b03795da351a79bcd975a014c193a"))
		(    4, uint256("0x0046adb7661104fc051b1e609a658f4c876b15971a344ead57565ab01bdc810c"))
		(    5, uint256("0x4adedf38a03a837bca71a3b3d2b95c5a6a893406772d76935aabbcf56428667b"))
		(    6, uint256("0xdfbbc7f9930979dc9a67a6c08845563223cc41894b320fa17314c075ae550a65"))
		(    7, uint256("0x2fd9881f613b820965835a9f5387daa3a937beea7a2521eb107d80c6c85b2442"))
		(    8, uint256("0xe96d1e9cfae070d340bd3c87b419302fdfb00bfd3899df5277b6834e81f882be"))
		(    9, uint256("0x0332538734477f0eac83c85944237ad8877f9c1f3e5aaedfa840be5349eb7ca2"))
		(   10, uint256("0xa41d3e50fbb1074f1b564f0a45983c9d29ee310424bae5b01ac48d9414f7aefe"))
		(  100, uint256("0xb94a1ba4a6af6e5708fecaca3d42a16483dcf95764eb5c20812086753900b755"))
		(  250, uint256("0xb7645cd397dd54020d00a8842cfb8e347b41752df883be34dc6dc998c35800a1"))
		(  500, uint256("0x3a367e5cf5d37e03b04d9e8e3686f08b100cbcb48c947a0e4da8145b7bc3397c"))
		(  750, uint256("0xdfd5fc0f982659f5b584c8b9a14b7c80ec35f447419180a5ae1bd33c202dd16f"))
		( 1000, uint256("0xb4806100eea38dd8831d7eb1d0cb598d7d9c00a79a7233b5171109009e7ebc3f"))
		( 1500, uint256("0x74f7ec8386050c439a8389e2a5c5e380d461573ccfbde9e090e2cb3be0dae80f"))
		( 2000, uint256("0x467ab3c4aa4c78f0f599627cde35f84bb7ed92e0aeb1bfc6c65075b6c08961dd"))
		( 3000, uint256("0x9f8af4154f0ea93e6c9e33f5e1f839f24644d8df2a019346c98cc06787b0bd00"))
		( 5000, uint256("0xb746dca3705d2c51348a7aee6c663f91818a56341b43f3a1602a7a0a38f434d4"))
		(10000, uint256("0xa252b2a27183b1645baeedfd79b612d4f33087fcfa9fd7bd9fb653511a3cccc6"))
		
        ;
		
    static const CCheckpointData data = {
        &mapCheckpoints,
        1397308211, // * UNIX timestamp of last checkpoint block
        20702,    // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        3000     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        (     0, uint256("0xd5710a00bd80c20cce47749caa03b48ae2940322c4658aa57bc941152f7c2df4"))
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1394638207,
        0,
        400
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (fTestNet) return true; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (fTestNet) return 0; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (fTestNet) return NULL; // Testnet has no checkpoints
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
