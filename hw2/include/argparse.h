#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdlib.h>
#include <string.h>
#include <argp.h>

#endif /* ifndef ARGPARSE_H */

/// Help documentation.
static char doc[] = "Simple curl implementation.";

/// Description of the arguments we accept.
static char args_doc[] = "[-dghop] -u <url>[:<port>] [message here]";

/// Array of options, and their associated keys, flags, and docstring.
/// We set the ARG section to 0 (NULL) since the message will come
/// AFTER the url, rather than directly following the argument.
static struct argp_option options[] = {
    /// NAME     KEY   ARG        FLAGS		DOC
    {"url",       'u', "URL",       0, "URL to connect to.  OPTIONAL :<port>."},
    {"delete",    'd', "FILE",      0, "Item to delete from URL."},
    {"get",       'g', NULL,        0, "Receive data from URL."},
    {"help",      'h', NULL,        0, "Print help and exit."},
    {"post",      'o', "FILE",      0, "Item to place at URL; defaults to empty string."},
    {"put",       'p', "FILE",      0, "Item to place at URL."},
    {0, 0, 0, 0, 0}};

/// We set these if or when the argument is called.
/// We also fill in the message when we get to it.
struct arguments
{
  char *url;
  char *delete;
  char *get;
  char *help;
  char *post;
  char *put;
  char *message;
  int message_index;
};

/// Parse a single option.
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  /// Get the input argument from argp_parse, which we
  /// know is a pointer to our arguments structure.
  struct arguments *arguments = (struct arguments *)state->input;
  /// Counter for the number of arguments we've set.  Static so we don't reset
  /// between calls.
  static int option_set = 0;

  /*///////////////////////////////////////////////////////////////////////
  /// We only want to allow 1 option to be set (excluding url).         ///
  /// If an option is set, and a second option is called we return      ///
  /// an error.  The help arg will set the counter (option_set) to -10, ///
  /// so that adding other args won't push the value of this above 1.   ///
  /// Probably overkill, but it will help ensure the proper error.      ///
  ///////////////////////////////////////////////////////////////////////*/

  switch (key)
  {
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
    /// When we reach our first non-option argument, store
    /// the index then "consume" the rest of the arguments
    /// by setting the next state to the end (i.e. argc).
    if (state->arg_num == 0)
    {
      /// Subtract 1 to get the current index.
      arguments->message_index = state->next - 1;
      state->next = state->argc;
    }
    break;
  case ARGP_KEY_END:
    if (arguments->url == NULL)
    {
      argp_failure(state, 1, 0, "\nRequired Arg:\n"
                                "\t--url/-u\n\n"
                                "See -h/--help for more information.");
      return ARGP_ERR_UNKNOWN;
    }
    else if (option_set > 1)
    {
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

static struct argp argp = {options, parse_opt, args_doc, doc};
