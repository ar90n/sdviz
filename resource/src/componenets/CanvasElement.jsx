import React from 'react';
import {Buffer} from 'buffer';
import LZ4 from 'lz4';
import ElementComponent from './ElementComponent';

function getImageSize( { width, height, format } ) {
    return 3 * width * height;
}

function uncompressLZ4( buffer, uncompressSize ) {
    const uncompBuffer = new Buffer( uncompressSize );
    const uncompBufferSize = LZ4.decodeBlock( buffer, uncompBuffer );

    const ab = new ArrayBuffer(uncompBufferSize);
    const view = new Uint8Array( ab );
    for( let i = 0;i < uncompBufferSize; ++i )
    {
        view[i] = uncompBuffer[i];
    }
    return view;
}

class CanvasElement extends ElementComponent {
    static get TYPE() {
        return 1;
    }

    constructor( props, context ) {
        super( props, context );
    }

    componentDidMount() {
        this.updateCanvas();
    }

    componentDidUpdate() {
        this.updateCanvas();
    }

    updateCanvas() {
        const ctx = this.refs.canvas.getContext( "2d" );
        for( var i = 0; i < this.value.commands.length; i++ )
        {
            let command = this.value.commands[i];
            if( command.func === 'setProperty' )
            {
                ctx[command.args[0]] = command.args[1];
            }
            else if( command.func === 'putImageData' )
            {
                const src_image = command.args[0];
                const left_position = command.args[1];
                const top_position = command.args[2];
                const opacity = 255 * command.args[3] | 0;

                const image_size = getImageSize( src_image );
                src_image.buffer = uncompressLZ4( src_image.buffer, image_size );

                const dst_image = ctx.createImageData( src_image.width, src_image.height );
                for( let i = 0; i < src_image.width * src_image.height; i++ )
                {
                    dst_image.data[ 4 * i + 0 ] = src_image.buffer[ 3 * i + 0 ];
                    dst_image.data[ 4 * i + 1 ] = src_image.buffer[ 3 * i + 1 ];
                    dst_image.data[ 4 * i + 2 ] = src_image.buffer[ 3 * i + 2 ];
                    dst_image.data[ 4 * i + 3 ] = opacity;
                }
                ctx.putImageData( dst_image, left_position, top_position );
            }
            else {
                ctx[command.func].apply( ctx, command.args );
            }
        }
    }

    render() {
         return ( <canvas ref="canvas" style={styles.canvas} width={this.value.width} height={this.value.height}/> );
    }
}

const styles = {
    canvas: {
        width: '100%',
        height: '100%'
    }
};

export default CanvasElement;
