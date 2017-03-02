#include <config.h>
/* include/config.h.  Generated from config.h.in by configure.  */
/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if we have the timegm() function */
#define HAVE_TIMEGM 1

/* Define if we have the zlib library for gzip */
#define HAVE_ZLIB 1

/* Define if we have the bz2 library for bzip2 */
#define HAVE_BZ2 1

/* Define if we have the lzma library for lzma/xz */
#define HAVE_LZMA 1

/* Define if we have the lz4 library for lz4 */
/* #undef HAVE_LZ4 */

/* These two are used by the statvfs shim for glibc2.0 and bsd */
/* Define if we have sys/vfs.h */
/* #undef HAVE_VFS_H */
#define HAVE_STRUCT_STATFS_F_TYPE 1

/* Define if we have sys/mount.h */
/* #undef HAVE_MOUNT_H */

/* Define if we have enabled pthread support */
/* #undef HAVE_PTHREAD */

/* If there is no socklen_t, define this for the netdb shim */
/* #undef NEED_SOCKLEN_T_DEFINE */

/* Check for getresuid() function and similar ones */
#define HAVE_GETRESUID 1
#define HAVE_GETRESGID 1
#define HAVE_SETRESUID 1
#define HAVE_SETRESGID 1

/* Check for ptsname_r() */
/* #undef HAVE_PTSNAME_R */

/* Define to the size of the filesize containing structures */
/* #undef _FILE_OFFSET_BITS */

/* Define the arch name string */
#define COMMON_ARCH "amd64"

/* The package name string */
#define PACKAGE "apt"

/* The version number string */
#define PACKAGE_VERSION "1.2.19"

/* The mail address to reach upstream */
#define PACKAGE_MAIL "APT Development Team <deity@lists.debian.org>"

#define APT_8_CLEANER_HEADERS
#define APT_9_CLEANER_HEADERS
#define APT_10_CLEANER_HEADERS

/* unrolling is faster combined with an optimizing compiler */
#define SHA2_UNROLL_TRANSFORM
#include <apt-pkg/cmndline.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/fileutl.h>

#include <private-main.h>

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <apti18n.h>


void InitLocale()							/*{{{*/
{
   setlocale(LC_ALL,"");
   textdomain(PACKAGE);
}
									/*}}}*/
void InitSignals()							/*{{{*/
{
   signal(SIGPIPE,SIG_IGN);
}
									/*}}}*/
void CheckIfSimulateMode(CommandLine &CmdL)				/*{{{*/
{
   // disable locking in simulation, but show the message only for users
   // as root hasn't the same problems like unreadable files which can heavily
   // distort the simulation.
   if (_config->FindB("APT::Get::Simulate") == true &&
	(CmdL.FileSize() == 0 ||
	 (strcmp(CmdL.FileList[0], "source") != 0 && strcmp(CmdL.FileList[0], "download") != 0 &&
	  strcmp(CmdL.FileList[0], "changelog") != 0)))
   {
      if (getuid() != 0 && _config->FindB("APT::Get::Show-User-Simulation-Note",true) == true)
         std::cout << _("NOTE: This is only a simulation!\n"
	    "      apt-get needs root privileges for real execution.\n"
	    "      Keep also in mind that locking is deactivated,\n"
	    "      so don't depend on the relevance to the real current situation!"
	 ) << std::endl;
      _config->Set("Debug::NoLocking",true);
   }
}
									/*}}}*/
void CheckIfCalledByScript(int argc, const char *argv[])		/*{{{*/
{
   if (unlikely(argc < 1)) return;

   if(!isatty(STDOUT_FILENO) &&
      _config->FindB("Apt::Cmd::Disable-Script-Warning", false) == false)
   {
      std::cerr << std::endl
                << "WARNING: " << flNotDir(argv[0]) << " "
                << "does not have a stable CLI interface. "
                << "Use with caution in scripts."
                << std::endl
                << std::endl;
   }
}
									/*}}}*/
