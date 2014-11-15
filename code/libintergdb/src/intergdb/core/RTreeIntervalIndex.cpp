#include <intergdb/core/RTreeIntervalIndex.h>

#include <intergdb/core/NetworkByteBuffer.h>
#include <spatialindex/tools/Tools.h>
#include <boost/filesystem.hpp>
#include <unordered_set>

using namespace SpatialIndex;
using namespace intergdb::core;
using namespace std;

auto_ptr<Region> RTreeIntervalIndex::
    IntervalData::getRegion(Timestamp const start, Timestamp const end,
                            Timestamp yStart, Timestamp yEnd)
{
    Timestamp lows[] = { start, yStart };
    Timestamp highs[] = { end, yEnd };
    return auto_ptr<Region>(new Region(lows, highs, 2));
}

auto_array<unsigned char> RTreeIntervalIndex::IntervalData::
    getBytes(uint32_t & size) const
{
    NetworkByteBuffer nbf;
    nbf << id_;
    nbf << vertex_;
    nbf << start_;
    nbf << end_;
    nbf.setAutoDealloc(false);
    size = nbf.getSerializedDataSize();
    return auto_array<unsigned char>(nbf.getPtr());
}

void RTreeIntervalIndex::IntervalData::load(SpatialIndex::IData const & data)
{
    uint32_t len;
    unsigned char * bytes;
    data.getData(len, &bytes);
    NetworkByteBuffer buf(bytes, (uint64_t)len);
    buf >> id_;
    buf >> vertex_;
    buf >> start_;
    buf >> end_;
}

void RTreeIntervalIndex::openOrCreate(std::string const & baseName)
{
    uint32_t const nbuffBlocks = 1024;
    uint32_t const blockSize = 4096;
    std::string name(baseName); // Rtree calls take non-const, seems like a bug
    if (!boost::filesystem::exists(baseName+".idx")) { // if file is not there
        disk_ = StorageManager::createNewDiskStorageManager(name, blockSize);
        buffer_ = StorageManager::createNewLRUEvictionsBuffer(*disk_, nbuffBlocks, false);
        id_type indexId;
        rtidx_.reset(RTree::createNewRTree(*buffer_, 0.8, 100, 100, 2, RTree::RV_RSTAR, indexId));
        assert(indexId==1);
    } else { // file is already there
        std::cout << "RTreeIntervalIndex::openOrCreate loadDiskStorageManager(" + name + ") called" << std::endl;
        try {
            disk_ = StorageManager::loadDiskStorageManager(name);
        } catch (...) {
            std::exception_ptr p = std::current_exception();
            //std::cout <<(p ? p.__cxa_exception_type()->name() : "null") << std::endl;
            //std::cout <<(p ? p.what() : "null") << std::endl;
            std::cout << "RTreeIntervalIndex::openOrCreate exception caught" << std::endl;        
            std::rethrow_exception(p);
        }
        buffer_  = StorageManager::createNewLRUEvictionsBuffer(*disk_, nbuffBlocks, false);
        rtidx_.reset(RTree::loadRTree(*buffer_, 1));
        IStatistics * stats;
        rtidx_->getStatistics(&stats);
        std::auto_ptr<IStatistics> statsPtr(stats);
        nextId_ = stats->getNumberOfData();
    }
}

RTreeIntervalIndex::~RTreeIntervalIndex()
{
    rtidx_.reset(0);
    delete buffer_;
    delete disk_;
}

void RTreeIntervalIndex::addInterval(BlockId id, VertexId vertex,
                                     Timestamp const & start, Timestamp const & end)
{
    IntervalData data(id, vertex, start, end);
    auto region = data.getRegion();
    uint32_t nBytes;
    auto_array<unsigned char> bytes = data.getBytes(nBytes);
    rtidx_->insertData(nBytes, bytes.get(), *region, nextId_++);
}

class BatchVisitor : public SpatialIndex::IVisitor
{
public:
    void visitNode(SpatialIndex::INode const & node) {}
    void visitData(std::vector<const SpatialIndex::IData*>& v) {}
    void visitData(SpatialIndex::IData const & data)
    {
        data_.load(data);
        VertexId id = data_.getVertex();
        vertices_.insert(id);
    }
    void collectResults(vector<VertexId> & results)
    {
        results.clear();
        results.reserve(vertices_.size());
        for (VertexId v : vertices_)
            results.push_back(v);
    }
private:
    RTreeIntervalIndex::IntervalData data_;
    unordered_set<VertexId> vertices_;
};

void RTreeIntervalIndex::queryBatch(Timestamp start, Timestamp end, std::vector<VertexId> & results)
{
    BatchVisitor visitor;
    std::auto_ptr<SpatialIndex::Region> region =  IntervalData::getRegion(start, end, 0.0, 10.0);
    rtidx_->intersectsWithQuery(*region, visitor);
    visitor.collectResults(results);
}


