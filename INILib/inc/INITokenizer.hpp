#ifndef INITOKENIZER_HPP
#define INITOKENIZER_HPP

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50 // or whatever you need
#define BOOST_MPL_LIMIT_MAP_SIZE 50 // or whatever you need

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#include <boost/msm/front/euml/operator.hpp>
#include <boost/msm/front/euml/state_grammar.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <list>

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;
using namespace msm::front::euml;

enum TokenType 
{
    Section,
    Comment,
    Key,
    Context,
    Path,
    String,
    Numeric,
    Value
};

struct Token {
    TokenType type;
    std::string value;
};

std::list<Token> tokens;

static char const* const token_string[] = { "Section", "Comment", "Key", "Context", "Path", "String", "Numeric", "Value" };

static char const* const state_names[] = { "Start", "Section", "TrailingWhitespace", "Comment", "Key",
                                            "Context", "WaitingAssignmentOperator", "WaitingValue", "Path",
                                            "String", "Numeric", "Value"};

// events
struct Error {};
struct ProcessCharacter
{
    ProcessCharacter(const unsigned char ch)
        : ch(ch)
    { }

    const unsigned char ch;
};

// front-end: define the FSM structure
struct Tokenizer_ : public msm::front::state_machine_def<Tokenizer_>
{
    // The list of FSM states
    struct Start_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Start_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Start_tag {};
    typedef msm::front::euml::func_state<Start_tag, Start_Entry, Start_Exit> Start;

    struct Comment_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Comment_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Comment_tag {};
    typedef msm::front::euml::func_state<Comment_tag, Comment_Entry, Comment_Exit> Comment;

    struct Section_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Section_Exit // why isn't State working ?
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            Section& state = fsm. template get_state<Section&>();
            tokens.emplace_back(TokenType::Section, state.alpha);
            state.alpha.clear();
        }
    };

    struct Section_tag {};
    struct Section : msm::front::euml::func_state<Section_tag, Section_Entry, Section_Exit>
    {
        std::string alpha; // section name
    };

    struct ProcAlpha
    {
        template <class Fsm, class Evt, class SourceState, class TargetState>
        void operator()(Evt const& evt, Fsm& fsm, SourceState& src, TargetState&)
        {
            src.alpha += evt.ch;
        }
    };

    struct ProcString
    {
        template <class Fsm, class Evt, class SourceState, class TargetState>
        void operator()(Evt const& evt, Fsm& fsm, SourceState& src, TargetState&)
        {
            src.string += evt.ch;
        }
    };

    struct ProcNumeric
    {
        template <class Fsm, class Evt, class SourceState, class TargetState>
        void operator()(Evt const& evt, Fsm& fsm, SourceState& src, TargetState&)
        {
            src.number = src.number * 10 + (evt.ch - '0');
        }
    };

    struct Redirect
    {
        template <class Fsm, class Evt, class SourceState, class TargetState>
        void operator()(Evt const& evt, Fsm& fsm, SourceState& src, TargetState&)
        {
            fsm.process_event(ProcessCharacter(evt.ch));
        }
    };

    struct Key_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Key_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            Key& state = fsm.template get_state<Key&>();
            tokens.emplace_back(TokenType::Key, state.alpha);
            state.alpha.clear();
        }
    };
    struct Key_tag {};
    struct Key : msm::front::euml::func_state<Key_tag, Key_Entry, Key_Exit>
    {
        std::string alpha; // key name
    };

    struct Context_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Context_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            Context& state = fsm.template get_state<Context&>();
            tokens.emplace_back(TokenType::Context, state.alpha);
            state.alpha.clear();
        }
    };
    struct Context_tag {};
    struct Context : msm::front::euml::func_state<Context_tag, Context_Entry, Context_Exit>
    {
        std::string alpha; // context name
    };

    struct TrailingWhitespace_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct TrailingWhitespace_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct TrailingWhitespace_tag {};
    typedef msm::front::euml::func_state<TrailingWhitespace_tag, TrailingWhitespace_Entry, TrailingWhitespace_Exit> TrailingWhitespace;

    struct WaitingAssignmentOp_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct WaitingAssignmentOp_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct WaitingAssignmentOp_tag {};
    typedef msm::front::euml::func_state<WaitingAssignmentOp_tag, WaitingAssignmentOp_Entry, WaitingAssignmentOp_Exit> WaitingAssignmentOp;

    struct WaitingValue_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct WaitingValue_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct WaitingValue_tag {};
    typedef msm::front::euml::func_state<WaitingValue_tag, WaitingValue_Entry, WaitingValue_Exit> WaitingValue;

    struct Path_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Path_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            Path& state = fsm.template get_state<Path&>();
            tokens.emplace_back(TokenType::Path, state.alpha);
            state.alpha.clear();
        }
    };
    struct Path_tag {};
    struct Path : msm::front::euml::func_state<Path_tag, Path_Entry, Path_Exit>
    {
        std::string alpha;
    };

    struct String_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            String& state = fsm.template get_state<String&>();
            state.alpha += "\"";
        }
    };
    struct String_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            String& state = fsm.template get_state<String&>();
            state.alpha += "\"";
            tokens.emplace_back(TokenType::String, state.alpha);
            state.alpha.clear();
        }
    };
    struct String_tag {};
    struct String : msm::front::euml::func_state<String_tag, String_Entry, String_Exit>
    {
        std::string alpha;
    };

    struct Numeric_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
        }
    };
    struct Numeric_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            Numeric& state = fsm.template get_state<Numeric&>();
            tokens.emplace_back(TokenType::Numeric, std::to_string(state.number));
            state.number = 0;
        }
    };
    struct Numeric_tag {};
    struct Numeric : msm::front::euml::func_state<Numeric_tag, Numeric_Entry, Numeric_Exit>
    {
        size_t number{};
    };

    struct Value_Entry
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM&, State&)
        { }
    };
    struct Value_Exit
    {
        template<class Event, class FSM, class State>
        void operator()(Event const& evt, FSM& fsm, State&)
        {
            Value& state = fsm.template get_state<Value&>();
            tokens.emplace_back(TokenType::Value, state.alpha);
            state.alpha.clear();
        }
    };
    struct Value_tag {};
    struct Value : msm::front::euml::func_state<Value_tag, Value_Entry, Value_Exit>
    {
        std::string alpha;
    };

    struct Final : public msm::front::terminate_state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&) { }
        template <class Event, class FSM>
        void on_exit(Event const&, FSM&) { }
    };

    struct AllOk : public msm::front::state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&) { }
        template <class Event, class FSM>
        void on_exit(Event const&, FSM&) { }
    };
    struct ErrorMode : public msm::front::terminate_state<>
    {
        template <class Event, class FSM>
        void on_entry(Event const&, FSM&) { }
        template <class Event, class FSM>
        void on_exit(Event const&, FSM&) { }
    };
    struct ErrorFound
    {
        template <class Fsm, class Evt, class SourceState, class TargetState>
        void operator()(Evt const& evt, Fsm& fsm, SourceState& src, TargetState&)
        {
            fsm.process_event(Error());
        }
    };
    typedef mpl::vector<Start, AllOk> initial_state;

    struct IsCommentStarter
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == ';';
        }
    };

    struct IsSectionOpener
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == '[';
        }
    };

    struct IsSectionCloser
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == ']';
        }
    };

    struct IsAlpha
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return std::isalpha(evt.ch) || (evt.ch == '_');
        }
    };

    struct IsContextOpener
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == '<';
        }
    };

    struct IsContextCloser
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == '>';
        }
    };

    struct IsWhitespace
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return std::isspace(evt.ch);
        }
    };

    struct IsAssignmentOp
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == '=';
        }
    };

    struct IsPathStarter
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == '/';
        }
    };

    struct IsDoubleQuotes
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == '\"';
        }
    };

    struct IsNumeric
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return std::string_view("0123456789").find(evt.ch) != std::string::npos;
        }
    };

    struct IsColon
    {
        template <class EVT, class FSM, class SourceState, class TargetState>
        bool operator()(EVT const& evt, FSM& fsm, SourceState& src, TargetState& tgt)
        {
            return evt.ch == ',';
        }
    };

    typedef Tokenizer_ p; // makes transition table cleaner

    struct transition_table : mpl::vector<
        //   Start               Event               Next                Action              Guard
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Start,              ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Start,              ProcessCharacter,   Comment,            none,               IsCommentStarter>,
        Row <Start,              ProcessCharacter,   Section,            none,               IsSectionOpener>,
        Row <Start,              ProcessCharacter,   Key,                Redirect,           IsAlpha>,
        Row <Start,              ProcessCharacter,   none,               none,               IsWhitespace>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Section,            ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Section,            ProcessCharacter,   TrailingWhitespace, none,               IsSectionCloser>,
        Row <Section,            ProcessCharacter,   none,               ProcAlpha,          IsAlpha>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <TrailingWhitespace, ProcessCharacter,   none,               ErrorFound,         none>,
        Row <TrailingWhitespace, ProcessCharacter,   Comment,            none,               IsCommentStarter>,
        Row <TrailingWhitespace, ProcessCharacter,   none,               none,               IsWhitespace>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Comment,            ProcessCharacter,   Final,              none,               none>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Key,                ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Key,                ProcessCharacter,   WaitingAssignmentOp,none,               IsWhitespace>,
        Row <Key,                ProcessCharacter,   Context,            none,               IsContextOpener>,
        Row <Key,                ProcessCharacter,   none,               ProcAlpha,          IsAlpha>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Context,            ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Context,            ProcessCharacter,   WaitingValue,       none,               IsAssignmentOp>,
        Row <Context,            ProcessCharacter,   WaitingAssignmentOp,none,               IsContextCloser>,
        Row <Context,            ProcessCharacter,   none,               ProcAlpha,          IsAlpha>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <WaitingAssignmentOp,ProcessCharacter,   none,               ErrorFound,         none>,
        Row <WaitingAssignmentOp,ProcessCharacter,   WaitingValue,       none,               IsAssignmentOp>,
        Row <WaitingAssignmentOp,ProcessCharacter,   none,               none,               IsWhitespace>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <WaitingValue,       ProcessCharacter,   none,               ErrorFound,         none>,
        Row <WaitingValue,       ProcessCharacter,   Path,               Redirect,           IsPathStarter>,
        Row <WaitingValue,       ProcessCharacter,   String,             none,               IsDoubleQuotes>,
        Row <WaitingValue,       ProcessCharacter,   Numeric,            Redirect,           IsNumeric>,
        Row <WaitingValue,       ProcessCharacter,   Value,              Redirect,           IsAlpha>,
        Row <WaitingValue,       ProcessCharacter,   none,               none,               IsWhitespace>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Path,               ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Path,               ProcessCharacter,   TrailingWhitespace, none,               IsWhitespace>,
        Row <Path,               ProcessCharacter,   none,               ProcAlpha,          Or_
                                                                                             <
                                                                                                 Or_<IsPathStarter, IsAlpha>,
                                                                                                 Or_<IsCommentStarter, IsNumeric>
                                                                                             >
            >,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <String,             ProcessCharacter,   none,               ProcAlpha,          none>,
        Row <String,             ProcessCharacter,   TrailingWhitespace, none,               IsDoubleQuotes>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Numeric,            ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Numeric,            ProcessCharacter,   Comment,            none,               IsCommentStarter>,
        Row <Numeric,            ProcessCharacter,   TrailingWhitespace, none,               IsWhitespace>,
        Row <Numeric,            ProcessCharacter,   none,               ProcNumeric,        IsNumeric>,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <Value,              ProcessCharacter,   none,               ErrorFound,         none>,
        Row <Value,              ProcessCharacter,   Comment,            none,               IsCommentStarter>,
        Row <Value,              ProcessCharacter,   TrailingWhitespace, none,               IsWhitespace>,
        Row <
             Value,              ProcessCharacter,   none,               ProcAlpha,          Or_<IsAlpha, IsColon>
            >,
        //  +-------------------+-------------------+-------------------+-------------------+-------------------+
        Row <AllOk,              Error,              ErrorMode,          none,               none>
    > {};

    // Replaces the default no-transition response.
    template <class FSM, class Event>
    void no_transition(Event const& e, FSM& fsm, int state)
    {
        fsm.process_event(ErrorFound());
    }
};

typedef msm::back::state_machine<Tokenizer_> Tokenizer;

#endif // INITOKENIZER_HPP