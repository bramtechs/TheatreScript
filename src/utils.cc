#include "theatre/utils.hh"
#include "theatre_script.hh"

namespace theatre
{
    class IgnoreOutputStreamBuffer : public std::streambuf {
    public:
        IgnoreOutputStreamBuffer() {}

    protected:
        // Override overflow to discard all characters
        int overflow(int c) override {
            // Simply return success to indicate that characters were successfully discarded
            return traits_type::not_eof(c);
        }
    };

    class IgnoredOutputStream : public std::ostream {
    public:
        IgnoredOutputStream() : std::ostream(&buffer_), buffer_() {}

    private:
        IgnoreOutputStreamBuffer buffer_;  // Custom stream buffer
    };

    static std::ostream& InitDebugStream()
    {
        if (VERBOSE_LOGGING) {
            return std::cout;
        } else {
            static IgnoredOutputStream ignored;
            return ignored;
        }
    }

    std::ostream& cdebug = InitDebugStream();
};