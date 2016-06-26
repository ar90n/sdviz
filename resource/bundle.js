var ResourceEmbedder = require('resource-embedder');
var fs = require('fs');

var embedder = new ResourceEmbedder('./index.html', { 'threshold' : '4096KB' } );

embedder.get(function (markup) {
  fs.writeFileSync('./dist/index_bundle.html', markup);
});
