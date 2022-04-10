// vim: cin:sts=4:sw=4 
#include "paste.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>
#include "http.h"
#include <curl/curl.h>

#define APIKEY "9efa8584c169af9fc3764af880009687"
const char api_url[] = "https://pastebin.com/api/api_post.php";
const char res_url[] = "https://pastebin.com";

char* upload_to_pastebin(const char* restrict nickname, const char* restrict pastename, const char* restrict text) {

    if (!curl_initialized) { curl_global_init(CURL_GLOBAL_ALL); curl_initialized = true; }

    CURL* esc = curl_easy_init();
    if (!esc) return NULL;

    if (!text) return NULL;

    char staticpost[8192];
    char* post = staticpost;    

    char* name_escaped = curl_easy_escape(esc, pastename, 0);
    char* content_escaped = curl_easy_escape(esc, text, 0);
    curl_easy_cleanup(esc); //this esc handle is only used for escaping
    
    int r = snprintf(staticpost,8192,"api_dev_key=" APIKEY "&api_option=paste&api_paste_code=%s&api_paste_name=%s",content_escaped,name_escaped);

    if (r > 8192) { post = malloc(r+1); snprintf(post,r+1,"api_dev_key=" APIKEY "&api_option=paste&api_paste_code=%s&api_paste_name=%s",content_escaped,name_escaped); };

    char* restext = make_http_request(api_url,post,NULL,0);

    curl_free(content_escaped);
    curl_free(name_escaped);

    return restext;
}
