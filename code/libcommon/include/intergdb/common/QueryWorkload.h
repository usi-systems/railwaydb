#pragma once

#include <intergdb/common/Attribute.h>
#include <intergdb/common/QuerySummary.h>
#include <intergdb/common/Query.h>

#include <vector>
#include <unordered_map>

namespace intergdb { namespace common
{
    class QueryWorkload
    {
    public:
        QueryWorkload()
        {}

        QueryWorkload(std::vector<Attribute> const & attributes)
            : totalQueries_(0)
        {
            for (auto &attribute : attributes)
            {
                attributes_.push_back(&attribute);
                nameToAttribute_[attribute.getName()] = &attribute;
            }
        }

        QueryWorkload(std::vector<Attribute> const & attributes,
                      std::vector<QuerySummary> const & queries)
            : QueryWorkload(attributes)
        {
            queries_ = queries;
        }

        void addAttribute(Attribute const & attribute)
        {
            attributes_.push_back(&attribute);
        }

        Attribute const & getAttribute(int index) const
        {
            return *attributes_.at(index);
        }

        std::vector<Attribute const *> const & getAttributes() const
        {
            return attributes_;
        }

        void addQuerySummary(QuerySummary const & query)
        {
            queries_.push_back(query);
        }

        std::vector<QuerySummary> const & getQuerySummaries() const
        {
            return queries_;
        }

        std::vector<QuerySummary> & getQuerySummaries()
        {
            return queries_;
        }

        std::string toString() const;

        void addQuery(Query const & q);

        double getFrequency(QuerySummary const & s) const;

        void setFrequency(QuerySummary const & s, double f);

    private:
        std::vector<Attribute const *> attributes_;
        std::vector<QuerySummary> queries_;
        std::unordered_map<Query, QuerySummary> summaries_;
        std::unordered_map<QuerySummary, double> counts_;
        std::unordered_map<std::string, const Attribute *> nameToAttribute_;
        double totalQueries_;
    };
} } /* namespace */

