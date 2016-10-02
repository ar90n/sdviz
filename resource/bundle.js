var ResourceEmbedder = require('resource-embedder');
var fs = require('fs');

var embedder = new ResourceEmbedder('./index.html', { 'threshold' : '4096KB' } );
var prefix_data_str = "unsigned char __dist_index_bundle_html[] = {";
var suffix_data_str = "\n};\nunsigned int __dist_index_bundle_html_len = ";

function toUTF8Array(str) {
    var utf8 = [];
    for (var i=0; i < str.length; i++) {
        var charcode = str.charCodeAt(i);
        if (charcode < 0x80) utf8.push(charcode);
        else if (charcode < 0x800) {
            utf8.push(0xc0 | (charcode >> 6), 
                      0x80 | (charcode & 0x3f));
        }
        else if (charcode < 0xd800 || charcode >= 0xe000) {
            utf8.push(0xe0 | (charcode >> 12), 
                      0x80 | ((charcode>>6) & 0x3f), 
                      0x80 | (charcode & 0x3f));
        }
        // surrogate pair
        else {
            i++;
            // UTF-16 encodes 0x10000-0x10FFFF by
            // subtracting 0x10000 and splitting the
            // 20 bits of 0x0-0xFFFFF into two halves
            charcode = 0x10000 + (((charcode & 0x3ff)<<10)
                      | (str.charCodeAt(i) & 0x3ff));
            utf8.push(0xf0 | (charcode >>18), 
                      0x80 | ((charcode>>12) & 0x3f), 
                      0x80 | ((charcode>>6) & 0x3f), 
                      0x80 | (charcode & 0x3f));
        }
    }
    return utf8;
}

embedder.get(function (markup) {
    var data_str = prefix_data_str;
    var byte_buffer = toUTF8Array(markup);
    for(let i = 0; i < byte_buffer.length; ++i )
    {
        if( (i % 12) == 0 )
        {
            if( 0 < i )
            {
                data_str += ',';
            }
            data_str += '\n  ';
        }
        else
        {
            data_str += ', ';
        }
        data_str += "0x" + ("0" + byte_buffer[i].toString(16)).substr(-2,2);
    }
    data_str +=  suffix_data_str + byte_buffer.length.toString() + ";";
    fs.writeFileSync('./dist/index_bundle.html', markup);
    fs.writeFileSync('./dist/index_bundle.html.h', data_str);
});
