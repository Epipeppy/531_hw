#include <argparse.h>


/// Description of the arguments we accept.
static char args_doc[] = "[-dghop] -u <url>[:<port>] [message here]";

/// Array of options, and their associated keys, flags, and docstring.
/// We set the ARG section to 0 NULL since the message will come 
/// AFTER the url, rather than directly following the argument.
static struct argp_option options[] = {
  /// NAME     KEY   ARG        FLAGS		DOC
  {"url",     'u',  "URL",      0,  "URL to connect to.  OPTIONAL :<port>."},
  {"delete",  'd',  NULL,       0,  "Item to delete from URL."},
  {"get",     'g',  NULL,       0,  "Receive data from URL."},
  {"help",    'h',  NULL,       0,  "Print help and exit."},
  {"post",    'o',  NULL,       0,  "Item to place at URL; defaults to empty string."},
  {"put",     'p',  NULL,       0,  "Item to place at URL."},
  {0, 0, 0, 0, 0}
};

/// We set these if the argument is called.
struct arguments {
  char *url, *delete, *get, *help, *post, *put;
  char **message;
};


/// Counter for the number of arguments we've set.
int option_set = 0;

/// Parse a single option.
static error_t parse_opt (int key, char* arg, struct argp_state* state) {
  /// Get the input argument from argp_parse, which we
  /// know is a pointer to our arguments structure.
  struct arg_parser_input* input = (struct arg_parser_input*) state->input;
  struct arguments* args = input->args;

  /*///////////////////////////////////////////////////////////////////////
  /// We only want to allow 1 option to be set (excluding url).         ///
  /// If an option is set, and a second option is called we return      ///
  /// an error.  The help arg will set the counter (option_set) to -10, ///
  /// so that adding other args won't push the value of this above 1.   ///
  /// Probably overkill, but it will help ensure the proper error.      ///
  ///////////////////////////////////////////////////////////////////////*/

  switch (key) {
    case 'u':
      arguments->url = arg;
      break;
    case 'd':
      arguments->delete = arg;
      option_set += 1;
      break;
    case 'h':
      arguments->help = arg;
      option_set = -10;
      break;
    case 'g':
      arguments->get = arg;
      option_set += 1;
      break;
    case 'o':
      arguments->post = arg;
      option_set += 1;
      break;
    case 'p':
      arguments->put = arg;
      option_set += 1;
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num == 0) {
        arguments->message = &state->argv[state->next - 1];
        state->next = state->argc;
      }
    case ARGP_KEY_END:
      if (arguments->url == NULL) {
          argp_failure(state, 1, 0, "\nRequired Arg:\n"
                     "\t--url/-u\n\n"
                     "See -h/--help for more information.");
          return ARGP_ERR_UNKNOWN;
      } else if (option_set > 1) {
          argp_failure(state, 1, 0, "\nError: too many arguments set\n\
                     Only 1 argument can be set per call (excluding \
                     --url/-u which is required).\n\
                     See -h/--help for more information.");
          return ARGP_ERR_UNKNOWN;
      }
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

//static struct argp argp = { options, parse_opt, args_doc };
static struct argp argp = { options, parse_opt, args_doc, doc };

int parse_arguments(int argc, char* argv[], struct arguments* args) {

  /// Defaults for our arguments.
  args.url	= NULL;
  args.delete	= NULL;
  args.get	= NULL;
  args.help	= NULL;
  args.post	= NULL;
  args.put	= NULL;

  struct arg_parser_input input = {
    .message_index = message_index,
    .args = args;
  };
  
  /// Function to parse command-line arguments.  Defined in argp.h.
  argp_parse(&argp, argc, argv, 0, 0, &input);
};
