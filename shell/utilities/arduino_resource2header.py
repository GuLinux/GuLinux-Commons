#!/usr/bin/env python3
import htmlmin
import jsmin
import gzip
import brotli
import argparse
import os
import mimetypes

def minify_content(mimetype, content):
    if mimetype in ['text/html', 'text/css']:
        print(f'   * HTML/CSS minification')
        return htmlmin.minify(content.decode(), remove_comments=True, remove_empty_space=True).encode()
    if mimetype == 'application/javascript':
        print(f'   * JS minification')
        return jsmin.jsmin(content.decode()).encode()
    return content

def sizeof_fmt(num, suffix="B"):
    for unit in ["", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi"]:
        if abs(num) < 1024.0:
            return f"{num:3.1f}{unit}{suffix}"
        num /= 1024.0
    return f"{num:.1f}Yi{suffix}"

def get_file_size(f):
    original_position = f.tell()
    file_size = f.seek(0,2)
    f.seek(original_position)
    return file_size

def process_file(input_file, output_file, name, add_relative_path='', progmem=True, minify=True, compress='gzip', webserver='none', append=False, header_guard=True):
    if not append:
        output_file.seek(0)
        output_file.truncate()

    file_content = input_file.read()
    file_size = get_file_size(input_file)

    file_original_path = input_file.name
    file_relative_path = os.path.relpath(file_original_path, add_relative_path)
    file_mimetype = mimetypes.guess_type(file_original_path)[0]
    file_original_name = os.path.basename(file_original_path)
    file_header_name = 'RESOURCES_' + file_original_name.replace('.', '_').upper() + '_H'
    total_content = 0

    print(f' - processing {input_file.name}, {file_mimetype}\n   * Original size: {sizeof_fmt(file_size)} ({file_size} bytes)')
    if minify:
        file_content = minify_content(file_mimetype, file_content)
    if compress == 'gzip':
        print(f'   * GZIP compression')
        file_content = gzip.compress(file_content, compresslevel=9)
    elif compress == 'br':
        print(f'   * Brotli compression')
        file_content = brotli.compress(file_content, brotli.MODE_TEXT, 11)
    else:
        file_content = file_content.encode()
    print(f'   * final size: {sizeof_fmt(len(file_content))} ({len(file_content)} bytes)')

    progmem = "PROGMEM" if progmem else ""

    if header_guard:
        output_file.write(f'''
    #ifndef {file_header_name}
    #define {file_header_name}
    ''')

    output_file.write(f'''
    // {file_relative_path}
    #ifndef __RESOURCES_WEBSERVER_TYPE_{webserver.upper()}
    #define __RESOURCES_WEBSERVER_TYPE_{webserver.upper()}
    #endif

    #ifdef __RESOURCES_WEBSERVER_TYPE_ESP

    #ifdef ESP8266
    #include <ESP8266WebServer.h>
    typedef ESP8266WebServer WebServer; 
    #endif

    #endif

    #ifdef __RESOURCES_WEBSERVER_TYPE_ESPASYNC
    #include <ESPAsyncWebServer.h>
    #else
    #include <WebServer.h>
    #endif

    namespace {name} {{
        const char *CONTENT_TYPE {progmem} = "{file_mimetype}";
        const char *ORIGINAL_NAME {progmem} = "{file_original_name}";
        const char *RELATIVE_PATH {progmem} = "/{file_relative_path}";
        const bool COMPRESSED = {'false' if compress == 'none' else 'true'};
        const char *COMPRESSION_ALGORITHM {progmem} = "{compress}";
        const size_t SIZE = {len(file_content)};
        const uint8_t DATA [] {progmem} = {{
        {', '.join([f'0x{element:02x}' for element in file_content])}
        }};

    #ifdef __RESOURCES_WEBSERVER_TYPE_ESP
        void handler{{
            if(COMPRESSED) {{
                webServer->sendHeader("Content-Encoding", COMPRESSION_ALGORITHM);
            }}
            webServer->send_P(200, CONTENT_TYPE, DATA, SIZE);
        }}
    #endif

    #ifdef __RESOURCES_WEBSERVER_TYPE_ESPASYNC
        void handler(AsyncWebServerRequest *request) {{
            auto response = request->beginResponse_P(200, CONTENT_TYPE, DATA, SIZE);
            if(COMPRESSED) {{
                response->addHeader("Content-Encoding", COMPRESSION_ALGORITHM);
            }}
            request->send(response);
        }}
    #endif

    }}
    ''')
    total_content += len(file_content)

    if header_guard:
        output_file.write('#endif\n')

    print(f'Total content: {sizeof_fmt(total_content)} ({total_content})')


if __name__ == "__main__":
    parser = argparse.ArgumentParser('Resources header creator')
    parser.add_argument('--header-guard', action=argparse.BooleanOptionalAction, default=True)
    parser.add_argument('--append', action=argparse.BooleanOptionalAction, default=False)
    parser.add_argument('--add-relative-path')
    parser.add_argument('--compress', choices=['none', 'gzip', 'br'], default='gzip')
    parser.add_argument('--webserver', choices=['none', 'ESP', 'ESPAsync'], default='none')
    parser.add_argument('--minify', action=argparse.BooleanOptionalAction, default=True)
    parser.add_argument('--progmem', action=argparse.BooleanOptionalAction, default=True)


    parser.add_argument('input', type=argparse.FileType('rb'))
    parser.add_argument('output', type=argparse.FileType('a'))
    parser.add_argument('name')


    args = parser.parse_args()
    process_file(args.input, args.output, args.name, args.add_relative_path, args.progmem, args.minify, args.compress, args.webserver, args.append, args.header_guard)

