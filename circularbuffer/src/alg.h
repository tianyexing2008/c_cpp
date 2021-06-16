#include "CircularBuffer.h"
#include <memory>
#define MAX_QUEUE_SIZE 30

struct EncframeinfoAlg
{
    uint32_t                         yuvDataLength;
    std::shared_ptr< unsigned char > yuvData;
    void *                           frameinfo;

    ~EncframeinfoAlg()
    {
        printf("destructor !!!!\n");
    }
};



class CAlg
{
public:
    static CAlg *instance();

    CAlg();
    ~CAlg();

    bool pushData(const std::shared_ptr<EncframeinfoAlg> &frame);

    bool getData(std::shared_ptr< EncframeinfoAlg >& frameinfo);

private:
    std::shared_ptr < CircularBuffer< std::shared_ptr<EncframeinfoAlg> >> mCircularBuffer;
};

