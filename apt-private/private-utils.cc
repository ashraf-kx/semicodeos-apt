#include <config.h>

#include <apt-pkg/configuration.h>
#include <apt-pkg/fileutl.h>

#include <private-utils.h>

#include <cstdlib>
#include <unistd.h>

// DisplayFileInPager - Display File with pager				/*{{{*/
void DisplayFileInPager(std::string const &filename)
{
   pid_t Process = ExecFork();
   if (Process == 0)
   {
      const char *Args[3];
      Args[1] = filename.c_str();
      Args[2] = NULL;
      if (isatty(STDOUT_FILENO) == 1)
      {
	 // likely installed, provided by sensible-utils
	 std::string const pager = _config->Find("Dir::Bin::Pager",
	       "sensible-pager");
	 Args[0] = pager.c_str();
	 execvp(Args[0],(char **)Args);
	 // lets try some obvious alternatives
	 Args[0] = getenv("PAGER");
	 if (Args[0] != NULL)
	    execvp(Args[0],(char **)Args);

	 Args[0] = "pager";
	 execvp(Args[0],(char **)Args);
      }
      // we could read the file ourselves, but… meh
      Args[0] = "cat";
      execvp(Args[0],(char **)Args);
      exit(100);
   }

   // Wait for the subprocess
   ExecWait(Process, "pager", false);
}
									/*}}}*/
// EditFileInSensibleEditor - Edit File with editor			/*{{{*/
void EditFileInSensibleEditor(std::string const &filename)
{
   pid_t Process = ExecFork();
   if (Process == 0)
   {
      // likely installed, provided by sensible-utils
      std::string const editor = _config->Find("Dir::Bin::Editor",
	    "sensible-editor");
      const char *Args[3];
      Args[0] = editor.c_str();
      Args[1] = filename.c_str();
      Args[2] = NULL;
      execvp(Args[0],(char **)Args);
      // the usual suspects we can try as an alternative
      Args[0] = getenv("VISUAL");
      if (Args[0] != NULL)
	 execvp(Args[0],(char **)Args);

      Args[0] = getenv("EDITOR");
      if (Args[0] != NULL)
	 execvp(Args[0],(char **)Args);

      Args[0] = "editor";
      execvp(Args[0],(char **)Args);
      exit(100);
   }

   // Wait for the subprocess
   ExecWait(Process, "editor", false);
}
									/*}}}*/
