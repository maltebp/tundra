
#include <tundra/core/string.hpp>

#include <tundra/core/assert.hpp>
#include "tundra/core/string-util.hpp"

namespace td {

    uint32 get_c_string_size(const char* string) {
        uint32 size = 0; 
        while( string[size] != '\0' ) {
            size++;
        }
        return size;
    }

    // Copies the C-string up until the null-terminator
    String::String(const char* c_string) {
        
        uint32 input_size = get_c_string_size(c_string);

        this->size = input_size;
        
        if( this->size > 0 ) {
            characters = new char[this->size + 1];
            for( uint32 i = 0; i < this->size; i++ ) {
                characters[i] = c_string[i];
            }
            characters[this->size] = '\0';
        }
    }

    String::String(char* c_string, uint32 size)
        :   characters(c_string), size(size)
    { }

    String::String(const String& other) {
        this->size = other.size;
        this->characters = new char[this->size + 1];
        for( uint32 i = 0; i < this->size; i++ ) {
            characters[i] = other.characters[i];
        }
        characters[this->size] = '\0';
    }

    String::~String() {
		if( characters != nullptr ) {
            delete[] characters;
        }
	}

    bool String::is_empty_or_whitespace() const {
		if( this->is_empty() ) return true;
        
        for( uint32 i = 0; i < this->size; i++ ) {
            if( characters[i] == ' ' ) continue;
            if( characters[i] == '\t' ) continue;
            if( characters[i] == '\n' ) continue;
            if( characters[i] == '\r' ) continue;
            return false;
        }

        return true;
	}

    bool String::operator==(const String& other) const {
        if( size != other.size ) return false;
        return *this == other.get_c_string();
	}

    bool String::operator!=(const String& other) const {
		return !(*this == other);
	}

    bool String::operator==(const char* c_string) const {
        
        // "test\0"
        // "test2\0"
        uint32 c_string_length = 0;
        while(c_string[c_string_length] != '\0') {
            if( c_string_length >= this->size ) return false; // c_string is longer than this
            if( c_string[c_string_length] != this->characters[c_string_length] )  return false;
            c_string_length++;
        }

        if( c_string_length != this->size ) return false; // this is longer than c_string
        
        return true;
	}

    bool String::operator!=(const char* c_string) const {
		return !(*this == c_string);
	}

    String& String::operator=(const String& other) {
        set(other.characters, other.size);
        return *this;
    }

    String& String::operator=(const char* other) {
        uint32 other_size = get_c_string_size(other);
        set(other, other_size);
        return *this;
    }

    String& String::operator+=(const String& other) {
        if( other.size == 0 ) return *this;
        
        char* new_c_string = concat_to_new(other.characters, other.size);
        this->characters = new_c_string;
        this->size = this->size + other.size;

        return *this;
    }

    String& String::operator+=(const char* c_string) {
        uint32 c_string_size = get_c_string_size(c_string);
        if( c_string_size == 0 ) return *this;
        
        char* new_c_string = concat_to_new(c_string, c_string_size);
        this->characters = new_c_string;
        this->size = this->size + c_string_size;

        return *this;
	}

    String String::operator+(const String& other) const {
        char* new_c_string = concat_to_new(other.characters, other.size);
        return String { new_c_string, this->size + other.size };
	}

    String String::operator+(const char* c_string) const {
        uint32 c_string_size = get_c_string_size(c_string);
		char* new_c_string = concat_to_new(c_string, c_string_size);
        return String { new_c_string, this->size + c_string_size }; 
	}

    void String::set(const char* c_string, uint32 size) {
        
        if( this->size > 0 && this->size < size) {
            delete[] this->characters;
        }

        if( size == 0 ) {
            this->characters = nullptr;
            this->size = 0;
            return;
        }

        if( this->size < size ) {
            this->characters = new char[size + 1];
        }

        // It may be worth considering if we should also re-allocate if there is
        // large a difference in the sizes of the target and source string

        for( uint32 i = 0; i < size; i++ ) {
            this->characters[i] = c_string[i];
        }

        this->characters[size] = '\0';
        this->size = size;
    }

    char* String::concat_to_new(const char* c_string, uint32 size) const {
                
        char* new_characters = new char[this->size + size + 1];
        
        for( uint32 i = 0; i < this->size; i++ ) {
            new_characters[i] = this->characters[i];
        }

        for( uint32 i = 0; i < size; i++ ) {
            new_characters[this->size + i] = c_string[i];
        }

        new_characters[this->size + size] = '\0';

        return new_characters;
    }


    String internal::ptr_to_string(const void* ptr) {
        return string_util::create_from_format("%p", ptr);
    }

    template<>
    String to_string<char>(const char& value) {
        const char value_buffer[2] { value, '\0' }; 
        return String { value_buffer };
    }

    template<>
    String to_string<unsigned char>(const unsigned char& value) {
        const char value_buffer[2] { (char)value, '\0' }; 
        return String { value_buffer };
    }

    template<>
    String to_string<signed char>(const signed char& value) {
        const char value_buffer[2] { value, '\0' }; 
        return String { value_buffer };
    }

    template<>
    String to_string<bool>(const bool& value) {
        return String { value ? "true" : "false" };
    }

    template<>
    String to_string<short>(const short& value) {
        return string_util::create_from_format("%hd", value);
    }

    template<>
    String to_string<int>(const int& value) {
        return string_util::create_from_format("%d", value);
    }

    template<>
    String to_string<long>(const long& value) {
        return string_util::create_from_format("%ld", value);
    }

    template<>
    String to_string<long long>(const long long& value) {
        return string_util::create_from_format("%lld", value);
    }

    template<>
    String to_string<unsigned short>(const unsigned short& value) {
        return string_util::create_from_format("%hu", value);
    }

    template<>
    String to_string<unsigned int>(const unsigned int& value) {
        return string_util::create_from_format("%u", value);
    }

    template<>
    String to_string<unsigned long>(const unsigned long& value) {
        return string_util::create_from_format("%lu", value);
    }

    template<>
    String to_string<unsigned long long>(const unsigned long long& value) {
        return string_util::create_from_format("%llu", value);
    }

    template<>
    String to_string<String>(const String& string) {
        return string;
    }

    template<>
    String to_string<char>(const char* string) {
        return String{string};
    }

}