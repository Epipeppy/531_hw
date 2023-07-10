#include <stdio.h>
#include <curl/curl.h>
#include "../include/argparse.h"

/// Return codes.
enum STATUS
{
	OK,		  // 0 everything worked.
	INIT_ERR, // 1 error initializing curl object.
	REQ_ERR,  // 2 error requesting information.
	OTHER_ERR // 3 catch all for remaining errors.
} Status;

void print_help(enum STATUS error);

int main(int argc, char *argv[])
{
	/// Create arguments struct.
	struct arguments args;

	/// Initialize and/or set default values.
	/// Success status is 0.  If an error occurs we will set a different status number.
	/// This will be our return value.
	Status = OK;
	args.url = NULL;
	args.delete = NULL;
	args.get = NULL;
	args.help = NULL;
	args.post = NULL;
	args.put = NULL;
	args.message = "";
	args.message_index = -1;

	// long http_resp_code;
	/// Will be used to set the operation for curl to perform.
	/// Defaulting to GET.
	CURLoption arg_opt = CURLOPT_HTTPGET;
	/// Pointer to curl object.
	CURL *curl;
	/// Curl response code; 0 (CURLE_OK) means no problems.
	CURLcode res;

	/// The argument parser.
	/// This will fill in our argument structure (args).
	argp_parse(&argp, argc, argv, 0, 0, &args);

	/*///////////////////////////////////////////////////////////////////
	/// The message will come at the end of the inputs.		  ///
	/// We will concatenate the remaining arguments into our message. ///
	/// Later we will curl_easy_encode() it to help take care of	  ///
	/// potentially riskier characters.				  ///
	/// Need to get the size of the message so we can allocate	  ///
	/// space for the args.message variable.  Using malloc will	  ///
	/// help us deal with large strings, whereas using		  ///
	/// args.message[mess_size + 1] will allocate from a smaller	  ///
	/// memory pool.						  ///
	///////////////////////////////////////////////////////////////////*/

	if ((strlen(args.message) != 0UL) && (args.message_index != -1))
	{
		int j = 0;
		size_t mess_size = 0;
		/// Increment until end of the message (i.e. argc).
		for (j = args.message_index; j < argc; j++)
		{
			mess_size += strlen(argv[j]);
		}

		/// Assign enough memory for the message.  Don't forget to free.
		/// The +1 here is for the string null terminator '\0'.
		args.message = malloc(sizeof(char) * (mess_size + 1));
		/// Copy in the first message to initialize the variable.
		j = args.message_index;
		strcpy(args.message, argv[j]);
		/// Start at j + 1 since we just copied arg[j].
		for (j = j + 1; j < argc; j++)
		{
			/// Add spaces since the argument parser skips them.
			strcat(args.message, " ");
			strcat(args.message, argv[j]);
		}

		printf("%s\n", args.message);
	}

	/// Check for -h/--help in command inputs, if it's there
	/// display the help and exit, ignoring all other inputs.
	/// If no arguments are given, print the help and exit.
	if (args.help != NULL)
	{
		print_help(Status);
	}

	if (args.delete != NULL)
	{
		/// No "official" DELETE request, but it is recommended to use
		/// a custom request, with the path to the item to delete.
		arg_opt = CURLOPT_CUSTOMREQUEST;
		/// Message must exist for DELETE request.
		if (strlen(args.message) == 0UL)
		{
			Status = OTHER_ERR;
			print_help(Status);
		}
	}

	if (args.get != NULL)
	{
		arg_opt = CURLOPT_HTTPGET;
	}

	if (args.post != NULL)
	{
		/// From the curl site:
		/// Using CURLOPT_POSTFIELDS implies setting CURLOPTPOST to 1.
		arg_opt = CURLOPT_POSTFIELDS;
	}

	if (args.put != NULL)
	{
		arg_opt = CURLOPT_UPLOAD;
		/// Message must exist for PUT request.
		if (strlen(args.message) == 0UL)
		{
			Status = OTHER_ERR;
			print_help(Status);
		}
	}

	/// Initialize curl object, then set options.
	curl = curl_easy_init();
	if (curl)
	{

		/*//////////////////////////////////////////////////////////////////////////////
		/// Be careful of user input.  Escape any special characters to prevent	     ///
		/// shell expansion.							     ///
		/// Could also turn off shell expansion by setting an environment variable.  ///
		/// Escaping any special characters helps protect against		     ///
		/// malicious inputs.							     ///
		/// Must use curl_free when done with string to free up memory.		     ///
		/// Auto-determine string length by passing 0 as the last argument.	     ///
		//////////////////////////////////////////////////////////////////////////////*/

		char *escaped_message;
		if (strlen(args.message) != 0UL) {
			escaped_message = curl_easy_escape(curl, args.message, 0);
		} else {
			escaped_message = args.message;
		}
		/// We don't access args.message after this, so we should be in
		/// the clear to free the memory here. Moving free() calls to before the return.

		/// Set url.
		curl_easy_setopt(curl, CURLOPT_URL, args.url);
		/// Follow redirects.
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		/// Resolve IP automatically.
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
		/// Display header with/before body.
		curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
		/// Data to send (i.e. the message).
		curl_easy_setopt(curl, CURLOPT_READDATA, escaped_message);

		/// Use the input arguments to choose the operation.
		if (arg_opt == CURLOPT_HTTPGET)
		{
			/// Default curl operation with no options is GET.
			/// We initialize arg_opt to this, but it can be
			/// "manually" set as well.
			curl_easy_setopt(curl, arg_opt, 1L);
		}
		else if (arg_opt == CURLOPT_CUSTOMREQUEST)
		{
			/// If DELETE is set.
			/// Need a "file path" for DELETE requests.
			char *url_message_path;
			/// To account for the '/' and string terminator '\0'
			/// we add +2 to the size.
			size_t path_size = strlen(args.url) + 2;
			path_size += strlen(escaped_message);
			/// Don't forget to free().
			url_message_path = malloc(sizeof(char) * path_size);
			strcpy(url_message_path, args.url);
			strcat(url_message_path, "/");
			strcat(url_message_path, escaped_message);
			curl_easy_setopt(curl, arg_opt, "DELETE");
			curl_easy_setopt(curl, CURLOPT_URL, url_message_path);
			/// Need to free here due to scope.
			free(url_message_path);
		}
		else if (arg_opt == CURLOPT_UPLOAD)
		{
			/// If PUT is set.
			curl_easy_setopt(curl, arg_opt, 1L);
		}
		else
		{
			/// Else, POST is set.
			curl_easy_setopt(curl, arg_opt, escaped_message);
		}
		/// Perform the curl.
		res = curl_easy_perform(curl);

		/// Display HTTP response code.
		/// Shown in the header.
		// curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_resp_code);

		if (res != CURLE_OK)
		{
			Status = REQ_ERR;
		}
		/// Free memory.
		curl_free(escaped_message);
	}
	else
	{
		Status = INIT_ERR;
	}

	/// Clean up.
	free(args.message);
	curl_easy_cleanup(curl);
	return Status;
}

void print_help(enum STATUS error)
{
	/// If user enters a malformed string, display the error message, otherwise just print help.
	if (error)
		printf("Error, incorrect format.\n\n");

	printf("Usage:\n hw [option] -u/--url <url>[:<port>=80] [message]\n\
	Simple curl implementation.  Sends an HTTP Request.\n\
	Displays HTTP code (e.g. 404, 500, etc.) followed by any content returned.\n\
	REQUIRES -u/--url followed by a space then the URL (or localhost).\n\
	Default option is a GET request.  Port is assumed to be 80 if none given.\n\
	Only one type of request supported per call (i.e. '-d -p' will cause an error).\n\
	\nOptions:\n\
	<url>[:<port>]\t\tURL curl will connect to.  Port will default to 80 if none given.\n\
	-d/--delete\t\tSend a DELETE request.  REQUIRES a message following the URL.\n\
	-g/--get\t\tSend a GET request.  This is the default if no options are provided.\n\
	-h/--help\t\tDisplay this help, then exit.  This will override all other options.\n\
	-o/--post\t\tSend a POST request with a message.  Message should follow the URL.\n\
	\t\t\tDefaults to empty string ('') if no message is supplied.\n\
	-p/--put\t\tSend a PUT request with a message.  REQUIRES a message following the URL.\n\
	-u/--url\t\tURL to connect to (can be localhost).  To specify a port, follow the URL\n\
	\t\t\twith a colon (':') then the desired port without spaces.  Assumes port 80 if none given.\n");
	exit(error);
}
