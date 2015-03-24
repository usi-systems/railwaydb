#pragma once

namespace intergdb { namespace core
{
    class Edge
    {
    public:
        Edge(VertexId from, VertexId to, Timestamp tm)
            : from_(from), to_(to), tm_(tm)
        {}

        VertexId getFromVertex() const
        {
            return from_;
        }

        VertexId getToVertex() const
        {
            return to_;
        }

        Timestamp getTime() const
        {
            return tm_;
        }

    private:
        VertexId from_;
        VertexId to_;
        Timestamp tm_;
    };

    class UEdge
    {
    public:
        UEdge(VertexId v, VertexId u, Timestamp tm)
            : v_(v), u_(u), tm_(tm)
        {}
        VertexId getFirstVertex() const
        {
            return v_;
        }
        VertexId getSecondVertex() const
        {
            return u_;
        }
        Timestamp getTime() const
        {
            return tm_;
        }

    private:
        VertexId v_;
        VertexId u_;
        Timestamp tm_;
    };
} } /* namespace */

