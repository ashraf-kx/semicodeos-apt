#include <apt-pkg/cachefile.h>
#include <json/json.h>
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <curl/curl.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>



static pkgPolicy *policy;

static void print_candidate(const pkgCache::PkgIterator &P,json_object *list)
{
   pkgCache::VerIterator current = P.CurrentVer();
   pkgCache::VerIterator candidate = policy->GetCandidateVer(P);

   /*Creating a json object*/
   json_object *JSON_one_Package = json_object_new_object();

   if(!candidate || !candidate.FileList())
      return;

   std::string archive, origin, component;
   pkgCache::VerFileIterator VF = candidate.FileList();
   //VF.File().
   // see InRelease for the fields
   // also available: Codename, Label
   if (VF.File().Archive())
      archive = VF.File().Archive();
   if(VF.File().Origin())
      origin = VF.File().Origin();
   if(VF.File().Component())
      component = VF.File().Component();

   if (candidate) {

      /*getting a json one package details*/
      json_object *FullName      = json_object_new_string(P.FullName().c_str());
      json_object *candidateJSON = json_object_new_string(candidate.VerStr());
      json_object *archiveJSON   = json_object_new_string(archive.c_str());
      json_object *originJSON    = json_object_new_string(origin.c_str());
      json_object *componentJSON = json_object_new_string(component.c_str());

      /*Store the data to an object*/
      json_object_object_add(JSON_one_Package,"package_name", FullName);
      json_object_object_add(JSON_one_Package,"candidate", candidateJSON);
      json_object_object_add(JSON_one_Package,"archive", archiveJSON);
      json_object_object_add(JSON_one_Package,"origin", originJSON);
      json_object_object_add(JSON_one_Package,"component", componentJSON);

      /*Adding the above created json strings to the array*/
      json_object_array_add(list,JSON_one_Package);
   }
   // FIXME: add examle about how to access the pkgRecord via
   // pkgTagFile etc
}



/* holder for curl fetch */
struct curl_fetch_st {
   char *payload;
   size_t size;
};

/* callback for curl fetch */
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
   size_t realsize = size * nmemb;                             /* calculate buffer size */
   struct curl_fetch_st *p = (struct curl_fetch_st *) userp;   /* cast pointer to fetch struct */

   /* expand buffer */
   p->payload = (char *) realloc(p->payload, p->size + realsize + 1);

   /* check buffer */
   if (p->payload == NULL) {
     /* this isn't good */
     fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
     /* free buffer */
     free(p->payload);
     /* return */
     return -1;
   }

   /* copy contents to buffer */
   memcpy(&(p->payload[p->size]), contents, realsize);

   /* set new buffer size */
   p->size += realsize;

   /* ensure null termination */
   p->payload[p->size] = 0;

   /* return size */
   return realsize;
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
   CURLcode rcode;                   /* curl result code */

   /* init payload */
   fetch->payload = (char *) calloc(1, sizeof(fetch->payload));

   /* check payload */
   if (fetch->payload == NULL) {
       /* log error */
       fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url");
       /* return error */
       return CURLE_FAILED_INIT;
   }

   /* init size */
   fetch->size = 0;

   /* set url to fetch */
   curl_easy_setopt(ch, CURLOPT_URL, url);

   /* set calback function */
   curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);

   /* pass fetch struct pointer */
   curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);

   /* set default user agent */
   curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");

   /* set timeout */
   curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);

   /* enable location redirects */
   curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);

   /* set maximum allowed redirects */
   curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);

   /* fetch the url */
   rcode = curl_easy_perform(ch);

   /* return */
   return rcode;
}

int main(int argc,const char **argv)
{
    CURL *ch;                                               /* curl handle */
    CURLcode rcode;                                         /* curl result code */

    enum json_tokener_error jerr = json_tokener_success;    /* json parse error */

    struct curl_fetch_st curl_fetch;                        /* curl fetch struct */
    struct curl_fetch_st *cf = &curl_fetch;                 /* pointer to fetch struct */
    struct curl_slist *headers = NULL;                      /* http headers to send with request */

    /* url to test site */
    char *url = "http://127.0.0.1:8000/new_scEntry";  //http://jsonplaceholder.typicode.com/posts/

    /* init curl handle */
    if ((ch = curl_easy_init()) == NULL) {
        /* log error */
        fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session");
        /* return error */
        return 1;
    }

    /* set content type */
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    // headers = curl_slist_append(headers, "");

    /*Creating a json array*/
    json_object *JSON_List_Package = json_object_new_array();

    pkgInitConfig(*_config);
    pkgInitSystem(*_config, _system);

    pkgCacheFile cachefile;
    pkgCache *cache = cachefile.GetPkgCache();
    &cache;
    policy = cachefile.GetPolicy();

    if (cache == NULL || _error->PendingError()) {
        _error->DumpErrors();
        return 1;
    }

    for (pkgCache::GrpIterator grp = cache->GrpBegin(); grp != cache->GrpEnd(); grp++)
       for (pkgCache::PkgIterator p = grp.PackageList(); !p.end(); p = grp.NextPkg(p))
          print_candidate(p,JSON_List_Package);

    /*Now printing the json object*/
    //! printf ("The json object created: %s",json_object_to_json_string(JSON_List_Package));
    /*int JSON_size = json_object_object_length(JSON_List_Package);
    std::cout<<JSON_size<<std::endl;*/
     /* set curl options */
     curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "POST");
     curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
     curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(JSON_List_Package));

     /* fetch page and capture return code */
     rcode = curl_fetch_url(ch, url, cf);

     /* cleanup curl handle */
     curl_easy_cleanup(ch);

     /* free headers */
     curl_slist_free_all(headers);

     /* free json object */
     json_object_put(JSON_List_Package);

     /* check return code */
     if (rcode != CURLE_OK || cf->size < 1) {
         /* log error */
         fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s",
             url, curl_easy_strerror(rcode));
         /* return error */
         return 2;
     }

     /* check payload */
     if (cf->payload != NULL) {
         /* print result */
         printf("CURL Returned: \n%s\n", cf->payload);
         /* parse return */
         JSON_List_Package = json_tokener_parse_verbose(cf->payload, &jerr);
         /* free payload */
         free(cf->payload);
     } else {
         /* error */
         fprintf(stderr, "ERROR: Failed to populate payload");
         /* free payload */
         free(cf->payload);
         /* return */
         return 3;
     }

     /* check error */
     if (jerr != json_tokener_success) {
         /* error */
         fprintf(stderr, "ERROR: Failed to parse json string");
         /* free json object */
         json_object_put(JSON_List_Package);
         /* return */
         return 4;
     }
}
