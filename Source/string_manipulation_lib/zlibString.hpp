#ifndef ZLIB_STRING_HPP
#define ZLIB_STRING_HPP

#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <zlib.h>

#define Z_MAX_BUFFER_LENGTH (1.024e+9) /* 1 GIGABYTE */

namespace zlibString {
    inline int compress_array(const uint8_t* data, int len, uint8_t* output, int* out_len, int compressionlevel = Z_BEST_COMPRESSION) {
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        if (deflateInit(&stream, compressionlevel) != Z_OK) {
            return Z_STREAM_ERROR;
        }
        stream.avail_in = len;
        stream.next_in = const_cast<uint8_t*>(data);
        stream.avail_out = *out_len;
        stream.next_out = output;
        int ret = deflate(&stream, Z_FINISH);
        if (ret == Z_STREAM_ERROR) {
            return ret;
        }
        *out_len = stream.total_out;
        if (deflateEnd(&stream) != Z_OK) {
            return Z_STREAM_ERROR;
        }
        return Z_OK;
    }

    inline int decompress_array(uint8_t* data, int len, uint8_t* output, size_t max_allocation = Z_MAX_BUFFER_LENGTH) {
        size_t max_uncompressed_size = max_allocation;

        z_stream zstream;
        zstream.zalloc = Z_NULL;
        zstream.zfree = Z_NULL;
        zstream.opaque = Z_NULL;
        zstream.avail_in = 0;
        zstream.next_in = Z_NULL;
        int ret = inflateInit(&zstream);
        if (ret != Z_OK) {
            return Z_ERRNO;
        }

        zstream.avail_in = len;
        zstream.next_in = const_cast<unsigned char*>(data);

        size_t uncompressed_size = 0;
        int flush = Z_NO_FLUSH;

        do {
            zstream.avail_out = max_uncompressed_size;
            zstream.next_out = output;
            ret = inflate(&zstream, flush);
            if (ret == Z_STREAM_ERROR) {
                return Z_ERRNO;
            }

            size_t uncompressed_chunk_size = max_uncompressed_size - zstream.avail_out;
            uncompressed_size += uncompressed_chunk_size;

        } while (zstream.avail_out == 0);

        inflateEnd(&zstream);
        return Z_OK;
    }

	inline std::string compress_string(std::string str, int compressionlevel = Z_BEST_COMPRESSION)
    {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        if (deflateInit(&zs, compressionlevel) != Z_OK)
            throw(std::runtime_error("deflateInit failed while compressing."));
        zs.next_in = (Bytef*)str.data();
        zs.avail_in = str.size();
        int ret;
        char* outbuffer = new char[32768];
        std::string outstring;
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            ret = deflate(&zs, Z_FINISH);
            if (outstring.size() < zs.total_out) {
                outstring.append(outbuffer,
                    zs.total_out - outstring.size());
            }
        } while (ret == Z_OK);
        deflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            std::ostringstream oss;
            oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
            throw(std::runtime_error(oss.str()));
        }
        delete[] outbuffer;
        return outstring;
    }

	inline std::string decompress_string(std::string str)
    {
        z_stream zs;
        memset(&zs, 0, sizeof(zs));
        if (inflateInit(&zs) != Z_OK)
            throw(std::runtime_error("inflateInit failed while decompressing."));
        zs.next_in = (Bytef*)str.data();
        zs.avail_in = str.size();
        int ret;
        char* outbuffer = new char[32768];
        std::string outstring;
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            ret = inflate(&zs, 0);
            if (outstring.size() < zs.total_out) {
                outstring.append(outbuffer,
                    zs.total_out - outstring.size());
            }
        } while (ret == Z_OK);
        inflateEnd(&zs);
        if (ret != Z_STREAM_END) {
            std::ostringstream oss;
            oss << "Exception during zlib decompression: (" << ret << ") "
                << zs.msg;
            throw(std::runtime_error(oss.str()));
        }
        delete[] outbuffer;
        return outstring;
    }
};

#endif // ZLIB_STRING_HPP