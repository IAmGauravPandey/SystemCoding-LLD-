#include <bits/stdc++.h>
using namespace std;

class ByteIO {
    private:
        int blockSize;
        size_t currentPosition = 0;
        
        char *readBlock(size_t blockNum){
            char *blockData = static_cast<char *>(malloc(blockSize));
            bSeek(blockNum);
            bRead(blockNum);
            return blockData;
        }
        
        int bSeek(size_t blockNum);
        int bWrite(void *blockPtr);
        int bRead(void *blockPtr);
        int bBlockSize();
    public:
        ByteIO() : blockSize(bBlockSize()){}
        
        int write(void *dataPtr, size_t len){
            size_t blockNum = currentPosition/blockSize;
            size_t offsetInBlock = currentPosition%blockSize;
            
            if(offsetInBlock+len<=blockSize){
                //Data can fit in current block
                char* blockData = readBlock(blockNum);
                memcpy(&blockData[offsetInBlock],dataPtr,len);
                bWrite(blockData);
            }
            else{
                //Data spans multiple blocks
                size_t remainingBytes = len;
                char *data = static_cast<char*>dataPtr;
                
                while(remainingBytes>0){
                    char *blockData = readBlock(blockNum);
                    size_t bytesToWrite = min(remainingBytes,blockSize-offsetInBlock);
                    memcpy(&blockData[offsetInBlock],data,bytesToWrite);
                    data += bytesToWrite;
                    remainingBytes -= bytesToWrite;
                    offsetInBlock = 0;
                    ++blockNum;
                }
            }
            
            currentPosition += len;
            return len;
        }
        
        int read(void *dataPtr, size_t len){
            size_t blockNum = currentPosition/blockSize;
            size_t offsetInBlock = currentPosition%blockSize;
            
            char *data = static_cast<char *>(dataPtr);
            
            size_t bytesRead = 0;
            while(len>0){
                char *blockData = readBlock(blockNum);
                size_t bytesToRead = min(len,blockSize-offsetInBlock);
                memcpy(data,&blockData[offsetInBlock],bytesToRead);
                
                data += bytesToRead;
                len -== bytesToRead;
                bytesRead += bytesToRead;
                
                offsetInBlock = 0;
                ++blockNum;
            }
            
            currentPosition += bytesToRead;
            return bytesRead;
        }
        
        int seek(size_t location){
            if(location > currentPosition){
                size_t remainingBytes = location - currentPosition;
                size_t skipBlocks = remainingBytes/blockSize;
                currentPosition += bSeek(skipBlocks);
            } else {
                currentPosition += bSeek(location/blockSize);
            }
            
            return currentPosition;
        }
};

int main()
{
    
    return 0;
}
