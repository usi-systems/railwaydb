#ifndef INTERGDB_RUN_EXP_RUNEXP_H
#define INTERGDB_RUN_EXP_RUNEXP_H

#include <intergdb/run/Run.h>

namespace intergdb { namespace run { namespace exp {

class RunPopulateDBs : public Run
{
public:
    virtual void process();
};

class RunPopulateDBsForDifferentAlgs : public Run
{
public:
    virtual void process();
};

class RunPopulateDBsForExpirationHeapSizes : public Run
{
public:
    virtual void process();
};

class RunPopulateDBsForBlockSizes : public Run
{
public:
    virtual void process();
};

class RunPopulateDBsForSkews : public Run
{
public:
    virtual void process();
};

class RunPopulateDBsForCandidates : public Run
{
public:
    virtual void process();
};

class RunPopulateTweetDBs : public Run
{
public:
    virtual void process();
};

class RunPopulateTweetPG : public Run
{
public:
    virtual void process();
};

class RunBasicTimeRange : public Run
{
public:
	virtual void process();
};

class RunQueryTimeForDifferentAlgs : public Run
{
public:
    virtual void process();
};

class RunQueryTimeForExpirationHeapSizes : public Run
{
public:
    virtual void process();
};

class RunQueryTimeForBlockSizes : public Run
{
public:
    virtual void process();
};

class RunQueryTimeForSkews : public Run
{
public:
    virtual void process();
};

class RunQueryTimeForHops : public Run
{
public:
    virtual void process();
};

class RunQueryTimeForTweets : public Run
{
public:
    virtual void process();
};

class RunQueryTimeForHopsWithTweetDB : public Run
{
public:
    virtual void process();
};




}}}

#endif /* INTERGDB_RUN_EXP_RUNEXP_H */
