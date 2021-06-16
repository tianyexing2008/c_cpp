#include "alg.h"

CAlg::CAlg()
{
    mCircularBuffer = std::make_shared< CircularBuffer< std::shared_ptr<EncframeinfoAlg>> >(MAX_QUEUE_SIZE);
}

CAlg::~CAlg()
{

}

CAlg *CAlg::instance()
{
    static CAlg alg;
    return &alg;
}

bool CAlg::pushData(const std::shared_ptr<EncframeinfoAlg> &frame)
{
    if(mCircularBuffer->getCurrentSize() < MAX_QUEUE_SIZE)
    {
        mCircularBuffer->putData(frame);
        return true;
    }
    return false;
}


bool CAlg::getData(std::shared_ptr< EncframeinfoAlg >& frameinfo)
{
    if(mCircularBuffer->getCurrentSize() > 0)
    {
        mCircularBuffer->getData(frameinfo);
        return true;        
    }
    return false;
}
