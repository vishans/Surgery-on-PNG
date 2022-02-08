
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

namespace helper {

   std::string __getHexValue(int i) {
        if (i < 10) return std::to_string(i);

        switch (i){

        case 10: return "a";

        case 11: return "b";

        case 12: return "c";

        case 13: return "d";

        case 14: return "e";

        case 15: return "f";

        }
    }

    void printByte(char b, std::string end ="\n", std::string between = "") {

        for (int i = sizeof(b) * 7; i >= 0; i--) {
            bool aBit = b & (1 << i);
            std::cout << aBit;
            std::cout << between;

        }
        std::cout << end;

    }


    int getIntfromByte(char b) {
        int ans = 0;
        for (int i = sizeof(b) * 7; i >= 0; i--) {
            bool aBit = b & (1 << i);


            ans += (int)aBit * (1 << i);


        }
        return ans;
    }

    std::string getHexFromByte(char b) {

        int upper = (b & 0xF0)>>4;
        int lower = b & 0x0F;

        return __getHexValue(upper) + __getHexValue(lower);

    }
}


constexpr char c(int arg) noexcept
{
    return static_cast<char>(arg);
}


class PNG {

private:
    std::ifstream stream;
    std::vector<char> rawdata;
    std::streampos fileSize;

    unsigned int bytePointer;

public:

    PNG(std::string path) {


        stream = std::ifstream(path, std::ifstream::binary);

        //get the file size
        fileSize = stream.tellg();
        stream.seekg(0, std::ios::end);
        fileSize = stream.tellg() - fileSize;
        stream.seekg(0, std::ios::beg);

        //put entire png file into vector
        rawdata = std::vector<char>((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

        bytePointer = 0;

        std::vector<char> v = { c(137), c(80), c(78), c(71), c(13), c(10), c(26), c(10) }; // PNG signature. All valid PNG files start with these same 8 bytes.
        if (!(this->getNextNByte(8) == v)) {
            std::cout << "thrw";
            throw std::runtime_error(std::string("Error: Not a Valid PNG"));

        }

        // if PNG is valid, reset bytePointer
        bytePointer = 0;
    }

    void showRawData(unsigned int width = 16) {
        unsigned int count = 0;
        for (auto i = rawdata.begin(); i != rawdata.end(); i++) {

            std::cout << *i;
            count++;

            if (count == width) {
                std::cout << "\n";
                count = 0;
            }


        }
    }

    long getFileSize() {
        return fileSize;
    }

    unsigned int getBytePointer() {
        return bytePointer;
    }

    std::vector<char> getNextNByte(unsigned int n) {
        auto start = rawdata.begin() + bytePointer;
        auto end = rawdata.begin() + n + bytePointer;

        // To store the sliced vector
        std::vector<char> result(n);

        // Copy vector using copy function()
        copy(start, end, result.begin());
        bytePointer = bytePointer + n;
        // Return the final sliced vector
        return result;
    }

    void outputNextNByteAsChar(unsigned int n, std::string end = "\n", std::string between = "") {
        std::vector<char> buffer = this->getNextNByte(n);

        for (auto i = buffer.begin(); i != buffer.end(); i++) {
            std::cout << *i;
            std::cout << between;

        }

        std::cout << end;


    }

    

    void outputNextNByteAsInt(unsigned int n, std::string end = "\n", std::string between = " ") {
        std::vector<char> buffer = this->getNextNByte(n);

        for (auto i = buffer.begin(); i != buffer.end(); i++) {
            std::cout << helper::getIntfromByte(*i);
            std::cout << between;

        }

        std::cout << end;



    }




    void outputNextNByteAsHex(unsigned int n, std::string end = "\n", std::string between = " ") {
        std::vector<char> buffer = this->getNextNByte(n);

        for (auto i = buffer.begin(); i != buffer.end(); i++) {
            std::cout << helper::getHexFromByte(*i);
            std::cout << between;
        }

        std::cout << end;


    }

    unsigned int skipNBytes(unsigned int n) {
        //to do: check if eof is reached and do sum abt it
        if (bytePointer + n <= this->fileSize) { //compiler might complain here
            bytePointer += n;
            return n;
        }

        return 0;
    }

    unsigned int get4ByteBufferAsBigEndianInt(std::vector<char> buffer) {
    unsigned int total = 0;
    char byte;
    int index_extract = 0;
    unsigned int one = 1;
    for (int index = 3; index >= 0;index --) {
        byte = buffer[index];
        
        for (int i = 0 ; i < 8; i++) {
            bool aBit = byte & (1<<i);

       
            total += (aBit << index_extract);
            index_extract++;

        }
       
        
        

    }

    return total;

}
    unsigned int get4ByteBufferAsBigEndianInt() {
        std::vector<char> buffer = this->getNextNByte(4);
        unsigned int total = 0;
        char byte;
        bool aBit;
        int index_extract = 0;
        unsigned int one = 1;
        for (int index = 3; index >= 0; index--) {
            byte = buffer[index];

            for (int i = 0; i < 8; i++) {
                aBit = byte & (1 << i);


                total += (aBit << index_extract);
                index_extract++;

            }




        }

        return total;

    }

    unsigned int rewindBytePointer(unsigned int n) {
        if (bytePointer - n < 0) return 0;
        this->bytePointer -= n;
        return n;
    }


};

    

int main()
{

    PNG p("C:\\Users\\VISHAN\\Desktop\\AK4.png");
    unsigned int chunkLength;

    //PNG p("C:\\Users\\VISHAN\\Desktop\\New Text Document (4).txt");
        p.outputNextNByteAsInt(8, "\n", " ");
        while (p.getBytePointer() < p.getFileSize()) {

            chunkLength = p.get4ByteBufferAsBigEndianInt();
            std::cout << "chunk length: " << chunkLength << "\n";

            std::cout << "chunk name: " ;
            p.outputNextNByteAsChar(4);

            std::cout << "skipping " << chunkLength << " bytes of chunk data\n";
            p.skipNBytes(chunkLength);

            std::cout << "CRC: ";
            p.outputNextNByteAsInt(4);

            std::cout << "\n";


        }
    
}

