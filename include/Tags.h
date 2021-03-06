//
// Created by Zachary on 2021/2/2.
//

#pragma once

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <map>
#include "Hex.h"
#include "TagID.h"
#include "Edition.h"

namespace nbtpp {

    class BaseTag {
    private:
        std::string name;
        size_t length;
        Edition edition = JAVA;
    public:
        unsigned char* raw;

        inline size_t getLength() const;

        /**
         * Set the length of data.
         * @param length
         */
        inline void setLength(size_t length);

        /**
         * It has no practical meaning.
         * Define it for passing compilation.
         */
        virtual void bind();

        const std::string& getName() const;

        void setName(const std::string& name);

        Edition getEdition() const;

        void setEdition(Edition edition);

    };

    /**
     * The class is a base class for nonspecific payload size that the prefix is a unsigned short.
     * e.g. String, List, Compound ...
     */
    class NonspecificShortTag : public BaseTag {
        unsigned short payload_size;

    };


    /**
     * The class is a base class for nonspecific payload size that the prefix is a signed integer.
     * e.g. ByteArray, IntArray ...
     */
    class NonspecificIntTag : public BaseTag {
        int payload_size;

    };

    template<typename T>
    struct List : public NonspecificIntTag, public std::vector<T> {

        const static unsigned char type_id = LIST;

        unsigned char contentItemTypeId;

        using std::vector<T>::vector;

        List(const int& size) : NonspecificIntTag() {
            this->reserve(size);
        }

        unsigned char getContentItemTypeId() const {
            return contentItemTypeId;
        }

        inline void setContentItemTypeId(unsigned char& contentItemTypeId) {
            this->contentItemTypeId = contentItemTypeId;
        }
    };

    struct Compound : public NonspecificShortTag {

        const static unsigned char type_id = COMPOUND;

        /**
         * Using a Content class to store the pointer and size of payload.
         */
        struct Content {
            unsigned char typeId;
            Payload payload;
            Edition edition;

            Content(const Edition& edition, const unsigned char& typeId, const Payload& payload);

            /**
             * A copy constructor.
             * @param ano Another one.
             */
            Content(const Compound::Content& ano);
        };

        std::map<std::string, Content> itemMap;

        std::map<std::string, Compound*> internalCompound;

        Compound();

        Compound(Compound* ano);

        Compound(const Content& content);

        /**
         * Add item to content map, it will allocate memory.
         * @param name The name of one item.
         * @param length The length of the data.
         * @param payload The payload.
         * @return the target data's pointer.
         */
        unsigned char* addItem(std::string& name, const char& typeId, const Payload& payload);


        /**
         * Find item by name, It just fits with ordinary tags.
         * E.g. Byte, Int, Short ...
         * @tparam Tag The target Tag.
         * @param name Target name.
         * @return The result.
         */
        template<typename Tag>
        Tag bindItemByName(const char* name) {
            auto it = itemMap.find(name);
            if (it != itemMap.end()) {
                Tag tag(it->second);
                tag.setName(name);
                return tag;
            } else {
                std::cout << "'" << name << "' not found in '" << getName() << "'" << std::endl;
                return NULL;
            }
        } 

        /**
         * Find the compound by name, it just fits with compound tag, DO NOT transfer others.
         * @tparam Tag The target Tag. It extends Compound.
         * @param name Target name.
         * @return The result.
         */
        template<typename Tag>
        Tag bindCompoundByName(const char* name) {
            Tag tag(internalCompound.find(name)->second);
            tag.bind();
            tag.setName(name);
            return tag;
        }

        /**
         * Find the list by name, it just fits with List Tag, DO NOT transfer others.
         * @tparam Tag The target Tag. It extends List.
         * @param name Target name.
         * @return The result.
         */
        template<typename Tag>
        List<Tag> bindListByName(const char* name) {
            List<Content> temp = *(List<Content>*) itemMap.find(name)->second.payload.ptr;

            List<Tag> result;
            result.setName(name);
            for (int i = 0; i < temp.size(); i++) {
                Tag tag(temp[i]);
                if (temp[0].typeId == COMPOUND) {
                    tag.bind();
                }
                result.push_back(tag);
            }

            result.setName(name);
            return result;
        }

        virtual void bind();

        size_t size();

    };


    /**
     * The Tag_End struct.
     */
    struct End : public BaseTag {
        const static unsigned char type_id = END;
        const static unsigned char payload_size = 0;

    };

    struct Byte : public BaseTag {

        const static unsigned char type_id = BYTE;
        const static unsigned char payload_size = 1;

        Byte();

        Byte(const unsigned char& payload);

        Byte(const Compound::Content&);

        unsigned char payload;

        int toInteger();

        friend std::ostream& operator<<(std::ostream& out, nbtpp::Byte& byte) {
            return out << static_cast<unsigned>(byte.payload);
        }

        size_t size();

    };

    struct Short : public BaseTag {
        const static unsigned char type_id = SHORT;
        const static unsigned char payload_size = 2;

        Short(const short&);

        Short(const Compound::Content&);

        short payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::Short& mShort) {
            return out << mShort.payload;
        }

        size_t size();

    };

    struct Int : public BaseTag {
        const static unsigned char type_id = INT;
        const static unsigned char payload_size = 4;

        Int();

        Int(const int&);

        Int(const Compound::Content&);

        int payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::Int& mInt) {
            return out << mInt.payload;
        }

        size_t size();

    };

    struct Long : public BaseTag {
        const static unsigned char type_id = LONG;
        const static unsigned char payload_size = 8;

        Long();

        Long(const long&);

        Long(const Compound::Content&);

        long payload;// Because the size of type long is different on different platforms.

        friend std::ostream& operator<<(std::ostream& out, nbtpp::Long& mLong) {
            return out << mLong.payload;
        }

        size_t size();

    };

    struct Float : public BaseTag {
        const static unsigned char type_id = FLOAT;
        const static unsigned char payload_size = 4;

        Float();

        Float(const float&);

        Float(const Compound::Content&);

        float payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::Float& mFloat) {
            return out << mFloat.payload;
        }

        size_t size();

    };

    struct Double : public BaseTag {
        const static unsigned char type_id = DOUBLE;
        const static unsigned char payload_size = 8;

        Double();

        Double(const double&);

        Double(const Compound::Content&);

        double payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::Double& mDouble) {
            return out << mDouble.payload;
        }

        size_t size();

    };

    struct ByteArray : public NonspecificIntTag {
        const static unsigned char type_id = BYTE_ARRAY;

        ByteArray();

        ByteArray(unsigned char*);

        ByteArray(const Compound::Content&);

        unsigned char* payload;

        size_t size();

        friend std::ostream& operator<<(std::ostream& out, nbtpp::ByteArray& mByteArray) {
            out << "[";
            for (int i = 0; i < mByteArray.size(); i++) {
                out << static_cast<unsigned>(mByteArray.payload[i]);
                if (i != mByteArray.size() - 1) {
                    out << ", ";
                }
            }
            return out << "]";
        }

        unsigned char& operator[](const unsigned int& position);

    };


    struct String : public NonspecificShortTag {
        const static unsigned char type_id = STRING;

        String();

        String(const std::string&);

        String(const char*);

        String(const Compound::Content&);

        std::string payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::String& mString) {
            return out << mString.payload;
        }

        size_t size();

        char& operator[](const unsigned int& position);

    };

    struct IntArray : public NonspecificIntTag {
        const static unsigned char type_id = INT_ARRAY;

        IntArray();

        IntArray(int*);

        IntArray(const Compound::Content&);

        int* payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::IntArray& mIntArray) {
            out << "[";
            for (int i = 0; i < mIntArray.size(); i++) {
                out << mIntArray.payload[i];
                if (i != mIntArray.size() - 1) {
                    out << ", ";
                }
            }
            return out << "]";
        }

        size_t size();

        int& operator[](const unsigned int& position);

    };

    struct LongArray : public NonspecificIntTag {
        const static unsigned char type_id = LONG_ARRAY;

        LongArray();

        LongArray(long*);

        LongArray(char*);

        LongArray(const Compound::Content&);

        long* payload;

        friend std::ostream& operator<<(std::ostream& out, nbtpp::LongArray& mLongArray) {
            out << "[";
            for (int i = 0; i < mLongArray.size(); i++) {
                out << mLongArray.payload[i];
                if (i != mLongArray.size() - 1) {
                    out << ", ";
                }
            }
            return out << "]";
        }

        long& operator[](const unsigned int& position);

        size_t size();

    };

}