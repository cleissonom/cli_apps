#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char method[10];
  char path[1024];
  char version[10];
  char host[1024];
  char headers[4096];
  char body[4096];
} HttpRequest;

typedef struct
{
  char *headers;
  char *body;
  size_t headers_size;
  size_t body_size;
} HttpResponse;

void parse_line(char *line, HttpRequest *request)
{
  if (strstr(line, "GET") == line || strstr(line, "POST") == line ||
      strstr(line, "PUT") == line || strstr(line, "DELETE") == line ||
      strstr(line, "PATCH") == line || strstr(line, "OPTIONS") == line ||
      strstr(line, "HEAD") == line)
  {
    sscanf(line, "%s %s %s", request->method, request->path, request->version);
  }
  else if (strstr(line, "Host:") == line)
  {
    sscanf(line, "Host: %s", request->host);
  }
  else if (strstr(line, "Content-Length:") == line ||
           strstr(line, "Content-Type:") == line ||
           strstr(line, "User-Agent:") == line)
  {
    strcat(request->headers, line);
    strcat(request->headers, "\r\n");
  }
  else
  {
    strcat(request->body, line);
    strcat(request->body, "\n");
  }
}

struct curl_slist *configure_http_headers(HttpRequest *request)
{
  struct curl_slist *headers = NULL;
  char header_buffer[4096];
  strcpy(header_buffer, request->headers);

  char *header_line = strtok(header_buffer, "\r\n");
  while (header_line != NULL)
  {
    headers = curl_slist_append(headers, header_line);
    header_line = strtok(NULL, "\r\n");
  }

  return headers;
}

size_t header_callback(void *ptr, size_t size, size_t nmemb,
                       HttpResponse *response)
{
  size_t new_len = response->headers_size + size * nmemb;
  response->headers = realloc(response->headers, new_len + 1);
  if (response->headers == NULL)
  {
    fprintf(stderr, "Error reallocating memory for headers\n");
    return 0;
  }
  memcpy(response->headers + response->headers_size, ptr, size * nmemb);
  response->headers_size = new_len;
  response->headers[new_len] = '\0';
  return size * nmemb;
}

size_t write_callback(void *ptr, size_t size, size_t nmemb, HttpResponse *response)
{
  size_t new_len = response->body_size + size * nmemb;
  response->body = realloc(response->body, new_len + 1);
  if (response->body == NULL)
  {
    fprintf(stderr, "Error reallocating memory for body\n");
    return 0;
  }
  memcpy(response->body + response->body_size, ptr, size * nmemb);
  response->body_size = new_len;
  response->body[new_len] = '\0';
  return size * nmemb;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr,
            "\033[1;31mUsage: %s <input_file.txt> <output_file.txt>\033[0m\n",
            argv[0]);
    return 1;
  }

  const char *input_filename = argv[1];
  const char *output_filename = argv[2];

  FILE *input_file = fopen(input_filename, "r");

  if (!input_file)
  {
    fprintf(stderr, "\033[1;31mError: Could not open file %s\033[0m\n",
            input_filename);
    return 1;
  }

  CURL *curl = curl_easy_init();

  if (!curl)
  {
    fprintf(stderr, "\033[1;31mError: Could not initialize curl\033[0m\n");
    fclose(input_file);
    return 1;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  HttpRequest request = {0};
  HttpResponse response = {0};

  while ((read = getline(&line, &len, input_file)) != -1)
  {
    parse_line(line, &request);
  }

  free(line);
  fclose(input_file);

  if (strlen(request.method) == 0 || strlen(request.path) == 0 ||
      strlen(request.version) == 0 || strlen(request.host) == 0)
  {
    fprintf(stderr, "\033[1;31mError: Missing required fields (Method, "
                    "Path, HTTP version, or Host)\033[0m\n");
    curl_easy_cleanup(curl);
    return 1;
  }

  if (strncmp(request.host, "http://", 7) != 0 &&
      strncmp(request.host, "https://", 8) != 0)
  {
    char temp_host[1024];
    snprintf(temp_host, sizeof(temp_host), "http://%s", request.host);
    strncpy(request.host, temp_host, sizeof(request.host) - 1);
    request.host[sizeof(request.host) - 1] = '\0';
  }

  char url[2048];
  snprintf(url, sizeof(url), "%s%s", request.host, request.path);
  curl_easy_setopt(curl, CURLOPT_URL, url);

  struct curl_slist *headers = configure_http_headers(&request);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  if (strcmp(request.method, "POST") == 0)
  {
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body);
  }
  else if (strcmp(request.method, "PUT") == 0 ||
           strcmp(request.method, "PATCH") == 0)
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.method);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body);
  }
  else if (strcmp(request.method, "DELETE") == 0 ||
           strcmp(request.method, "OPTIONS") == 0)
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  }
  else if (strcmp(request.method, "HEAD") == 0)
  {
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
  }

  response.headers = malloc(1);
  response.headers_size = 0;
  response.body = malloc(1);
  response.body_size = 0;

  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK)
  {
    fprintf(stderr, "\033[1;31mError: %s\033[0m\n", curl_easy_strerror(res));
  }
  else
  {
    FILE *output_file = fopen(output_filename, "w");
    if (output_file)
    {
      fprintf(output_file, "%s\n", response.headers);
      fprintf(output_file, "%s", response.body);
      fclose(output_file);
    }
    else
    {
      fprintf(stderr, "\033[1;31mError: Could not open output file %s\033[0m\n",
              output_filename);
    }
  }

  free(response.headers);
  free(response.body);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  printf("\033[1;32mRequest completed successfully!\033[0m\n");

  return 0;
}
