import React from 'react';
import {Buffer} from 'buffer';
import LZ4 from 'lz4';
import Konva  from 'konva';
import ElementComponent from './ElementComponent';

const MAX_SCALE = 15.0;
const UNIT_SCALE_DIST = 120;
const UNIT_WINDOW_DIST = 400;

function convertArrayView( view, format ) {
    switch( format ) {
        case SdvizImage.RGB_888:
        case SdvizImage.UINT_8:
            return new Uint8Array( view.buffer );
        case SdvizImage.UINT_16:
            return new Uint16Array( view.buffer );
    }

    throw new Error( "Unknown image format." );
}

function getChannelsPerPixel( format ) {
    switch( format ) {
        case SdvizImage.RGB_888:
            return 3;
        case SdvizImage.UINT_8:
        case SdvizImage.UINT_16:
            return 1;
    }

    throw new Error( "Unknown image format." );
}

function getBytesPerChannel( format ) {
    switch( format ) {
        case SdvizImage.RGB_888:
        case SdvizImage.UINT_8:
            return 1;
        case SdvizImage.UINT_16:
            return 2;
    }

    throw new Error( "Unknown image format." );
}

function uncompressLZ4( buffer, uncompressSize, format ) {
    const uncompBuffer = new Buffer( uncompressSize );
    const uncompBufferSize = LZ4.decodeBlock( buffer, uncompBuffer );

    const ab = new ArrayBuffer(uncompBufferSize);
    const view = new Uint8Array( ab );
    for( let i = 0;i < uncompBufferSize; ++i )
    {
        view[i] = uncompBuffer[i];
    }
    return convertArrayView( view, format );
}

function applyWindow( window_level, window_width, value ) {
    const min_value = window_level - window_width / 2;
    return 255 * Math.max( 0, Math.min( (value - min_value) / window_width, 1.0 ) );
}

function calcWindowInfo( image ) {
    if( getBytesPerChannel( image.format ) == 1 ) {
        return { width: 255, level: 127.5 }
    }

    let min_value = Number.MAX_VALUE;
    let max_value = Number.MIN_VALUE;

    const image_pixels = image.width * image.height;
    for( let i = 0; i < image_pixels; i++ )
    {
        min_value = Math.min( image.buffer[i], min_value );
        max_value = Math.max( image.buffer[i], max_value );
    }

    const window_width = max_value - min_value;
    const window_level = (max_value + min_value) / 2;
    return { width: window_width, level: window_level };
}

class SdvizImage extends Konva.Image {
    static get RGB_888() { return 0; }
    static get UINT_8() { return 1; }
    static get UINT_16() { return 2; }

    get window_level() {
        return this.wl;
    }

    get window_width() {
        return this.ww;
    }

    constructor( org_image, opacity, ctx ) {
        super();
        const image_size = this.getImageSize( org_image );
        org_image.buffer = uncompressLZ4( org_image.buffer, image_size, org_image.format );

        this.org_image = org_image;
        this.opacity = opacity;
        this.view_image = ctx.createImageData( org_image.width, org_image.height ); 

        const window_info = calcWindowInfo( this.org_image );
        this.wl= window_info.level;
        this.ww= window_info.width;
        this.org_window_width = window_info.width;
    }

    getImageSize( { width, height, format } ) {
        return getChannelsPerPixel(format) * getBytesPerChannel(format) * width * height;
    }

    update( window_level, window_width ) {
        this.wl= window_level || this.window_level;
        this.ww= Math.max(0, window_width) || this.window_width;

        const image_pixels = this.org_image.width * this.org_image.height;
        const pixel_step = getChannelsPerPixel( this.org_image.format );
        const channel_step = ( 1 < pixel_step ) ? 1 : 0;
        const has_alpha = ( pixel_step == 4) ? true : false;
        if( has_alpha ) {
            for( let i = 0; i < image_pixels; i++ )
            {
                this.view_image.data[ 4 * i + 0 ] = applyWindow( this.window_level, this.window_width, this.org_image.buffer[ pixel_step * i + 0 * channel_step ] );
                this.view_image.data[ 4 * i + 1 ] = applyWindow( this.window_level, this.window_width, this.org_image.buffer[ pixel_step * i + 1 * channel_step ] );
                this.view_image.data[ 4 * i + 2 ] = applyWindow( this.window_level, this.window_width, this.org_image.buffer[ pixel_step * i + 2 * channel_step ] );
                this.view_image.data[ 4 * i + 3 ] = this.org_image.buffer[ pixel_step * i + 3 * channel_step ];
            }
        }
        else {
            for( let i = 0; i < image_pixels; i++ )
            {
                this.view_image.data[ 4 * i + 0 ] = applyWindow( this.window_level, this.window_width, this.org_image.buffer[ pixel_step * i + 0 * channel_step ] );
                this.view_image.data[ 4 * i + 1 ] = applyWindow( this.window_level, this.window_width, this.org_image.buffer[ pixel_step * i + 1 * channel_step ] );
                this.view_image.data[ 4 * i + 2 ] = applyWindow( this.window_level, this.window_width, this.org_image.buffer[ pixel_step * i + 2 * channel_step ] );
                this.view_image.data[ 4 * i + 3 ] = this.opacity;
            }
        }

        return createImageBitmap( this.view_image ).then( ( bitmap ) => {
            this.setImage( bitmap );
            return Promise.resolve( this );
        });
    }

    calcWindowDisplacement( diff ) {
        return diff * this.org_window_width / UNIT_WINDOW_DIST;
    }
}

function createImageLayerPromise( command )
{
    const src_image = command.args[0];
    const left_position = command.args[1];
    const top_position = command.args[2];
    const opacity = 255 * command.args[3];

    const layer = new Konva.Layer();
    const ctx = layer.getContext();
    const image = new SdvizImage( src_image, opacity, ctx );
    return image.update().then( ( image_node ) => {
        layer.add( image_node );
        return Promise.resolve( layer );
    });
}

function createRectLayerPromise( command )
{
    const left       = command.args[0];
    const top        = command.args[1];
    const right      = command.args[2];
    const bottom     = command.args[3];
    const red        = command.args[4];
    const green      = command.args[5];
    const blue       = command.args[6];
    const line_width = command.args[7];
    const is_fill    = command.args[8];
    const is_dots    = command.args[9];

    const rect = new Konva.Rect({
        x : left,
        y : top,
        width: (right - left),
        height: (bottom - top),
        stroke: `rgb(${red},${green},${blue})`,
        strokeWidth : line_width
    });
    if( is_fill )
    {
        rect.fill( `rgb(${red},${green},${blue})` );
    }

    const layer = new Konva.Layer();
    layer.add( rect );
    return Promise.resolve( layer );
}

function createCircleLayerPromise( command )
{
    const center_x   = command.args[0];
    const center_y   = command.args[1];
    const radius     = command.args[2];
    const red        = command.args[3];
    const green      = command.args[4];
    const blue       = command.args[5];
    const line_width = command.args[6];
    const is_fill    = command.args[7];
    const is_dots    = command.args[8];

    const circle = new Konva.Circle({
        x : center_x,
        y : center_y,
        radius: radius,
        stroke: `rgb(${red},${green},${blue})`,
        strokeWidth : line_width
    });
    if( is_fill )
    {
        circle.fill( `rgb(${red},${green},${blue})` );
    }

    const layer = new Konva.Layer();
    layer.add( circle );
    return Promise.resolve( layer );
}

function createTextLayerPromise( command )
{
    const text_str  = command.args[0];
    const pos_x     = command.args[1];
    const pos_y     = command.args[2];
    const red       = command.args[3];
    const green     = command.args[4];
    const blue      = command.args[5];
    const font_size = command.args[6];

    const text = new Konva.Text({
        text: text_str,
        x : pos_x,
        y : pos_y,
        stroke: `rgb(${red},${green},${blue})`,
        fill: `rgb(${red},${green},${blue})`,
        fontSize : font_size
    });

    const layer = new Konva.Layer();
    layer.add( text );
    return Promise.resolve( layer );
}

function createLineLayerPromise( command )
{
    const points     = command.args[0];
    const red        = command.args[1];
    const green      = command.args[2];
    const blue       = command.args[3];
    const line_width = command.args[4];
    const is_fill    = command.args[5];
    const is_dots    = command.args[6];

    const line = new Konva.Line({
        points: points,
        stroke: `rgb(${red},${green},${blue})`,
        strokeWidth : line_width
    });
    if( is_fill )
    {
        line.fill( `rgb(${red},${green},${blue})` );
    }

    const layer = new Konva.Layer();
    layer.add( line );
    return Promise.resolve( layer );
}

function getModifierKeyStatus( shift, ctrl )
{
    const SHIFT_BIT = 0;
    const CTRL_BIT = 1;

    let status = 0;
    if( shift ) {
        status |= 1 << SHIFT_BIT;
    }
    if( ctrl ) {
        status |= 1 << CTRL_BIT;
    }

    return status;
}

function clipOffsetXY( offset_x, offset_y, scale, value_width, value_height, stage_width, stage_height )
{
    const res = {};

    if( ( scale * value_width ) < stage_width ) {
        res.x = value_width / 2;
    }
    else {
        const left_margin = stage_width / (2 * scale);
        const right_margin = value_width - left_margin;
        const clipped_offset_x = Math.min( right_margin, Math.max( offset_x, left_margin ) );
        res.x = clipped_offset_x;
    }

    if( ( scale * value_height ) < stage_height ) {
        res.y = value_height / 2;
    }
    else {
        const top_margin = stage_height / (2 * scale);
        const bottom_margin = value_height - top_margin;
        const clipped_offset_y = Math.min( bottom_margin, Math.max( offset_y, top_margin ) );
        res.y = clipped_offset_y;
    }

    return res;
}

function clipScale( scale, value_width, value_height, stage_width, stage_height, MAX_SCALE )
{
    const min_scale_x = Math.min( 1.0, stage_width / value_width );
    const min_scale_y = Math.min( 1.0, stage_height / value_height );
    const min_scale = Math.min( min_scale_x, min_scale_y );
    return Math.max( min_scale, Math.min( scale, MAX_SCALE ) );
}

class CanvasElement extends ElementComponent {
    static get TYPE() { return 1; }

    constructor( props, context ) {
        super( props, context );

        this.resizeListener = ( e ) => {
            this.updatePosition();
            this.stage.draw();
        };
        this.mousedownListener = ( e ) => {
            this.modifier_key_status = this.stage ? getModifierKeyStatus( e.evt.shiftKey, e.evt.ctrlKey ) : -1;
        };
        this.mousemoveListener = ( e ) => {
            if( !this.stage ) {
                return;
            }

            const curModifierKeyStatus = getModifierKeyStatus( e.shiftKey, e.ctrlKey );
            if( this.modifier_key_status != curModifierKeyStatus ) {
                this.modifier_key_status = -1;
                return;
            }

            const movement_y = -e.movementY;
            const movement_x = -e.movementX;
            if( this.modifier_key_status == 0 ) {
                const next_x = (movement_x / this.stage.scaleX()) + this.stage.offsetX();
                const next_y = (movement_y / this.stage.scaleY()) + this.stage.offsetY();
                const clipped_offset = clipOffsetXY( next_x,
                                                     next_y,
                                                     this.stage.scaleX(),
                                                     this.value.width,
                                                     this.value.height,
                                                     this.stage.width(),
                                                     this.stage.height() );
                this.stage.offset( clipped_offset );
                this.stage.draw();
            } else if( this.modifier_key_status == 1 ) {
                const next_scale = ( movement_y / UNIT_SCALE_DIST ) + this.stage.scaleY();
                const next_clipped_scale = clipScale( next_scale,
                                                      this.value.width,
                                                      this.value.height,
                                                      this.stage.width(),
                                                      this.stage.height(),
                                                      MAX_SCALE );
                this.stage.scale( { x: next_clipped_scale, y: next_clipped_scale });

                const clipped_offset = clipOffsetXY( this.stage.offsetX(),
                                                     this.stage.offsetY(),
                                                     this.stage.scaleX(),
                                                     this.value.width,
                                                     this.value.height,
                                                     this.stage.width(),
                                                     this.stage.height() );
                this.stage.offset( clipped_offset );
                this.stage.draw();
            } else if( this.modifier_key_status == 2 ) {
                const update_image_node_promises = this.stage.getLayers()
                    .reduce( ( prev, cur ) => {
                        Array.prototype.push.apply( prev, cur.getChildren( n => n.getClassName() === 'Image' ) ); 
                        return prev;
                    }, [] )
                    .map( node => {
                        const next_window_level = node.calcWindowDisplacement( movement_y ) + node.window_level;
                        const next_window_width = -node.calcWindowDisplacement( movement_x ) + node.window_width;
                        node.update( next_window_level, next_window_width );
                    });
                Promise.all( update_image_node_promises ).then( ( image_nodes ) => this.stage.draw() );
            }
        };
        this.mouseupListener = ( e ) => {
            this.modifier_key_status = -1;
        };

        this.stage = null;
        this.stage_container = null;
        this.modifier_key_status = -1;
    }

    componentDidMount() {
        this.stage = new Konva.Stage({ container: 'stage_container' });

        this.stage.on( 'mousedown', this.mousedownListener );
        document.addEventListener( 'mousemove', this.mousemoveListener );
        document.addEventListener( 'mouseup', this.mouseupListener );
        window.addEventListener( 'resize', this.resizeListener );

        this.componentDidUpdate();
    }

    componentDidUpdate() {
        this.stage.offset( { x: this.value.width / 2, y: this.value.height / 2 } );
        this.stage.setHeight( this.value.height );

        this.updateContent();
        this.updatePosition();
        this.stage.draw();
    }

    componentWillUnmount() {
        window.removeEventListener( 'resize', this.resizeListener );
        document.removeEventListener( 'mouseup', this.mouseupListener );
        document.removeEventListener( 'mousemove', this.mousemoveListener );
    }

    updatePosition() {
        this.stage.setWidth( this.stage_container.parentNode.clientWidth );

        this.stage.x( this.stage.width()/ 2 );
        this.stage.y( this.stage.height() / 2 );

        const next_clipped_scale = clipScale( this.stage.scaleX(),
                                              this.value.width,
                                              this.value.height,
                                              this.stage.width(),
                                              this.stage.height(),
                                              MAX_SCALE );
        this.stage.scale( {x: next_clipped_scale, y: next_clipped_scale } );

        const clipped_offset = clipOffsetXY( this.stage.offsetX(),
                                             this.stage.offsetY(),
                                             this.stage.scaleX(),
                                             this.value.width,
                                             this.value.height,
                                             this.stage.width(),
                                             this.stage.height() );
        this.stage.offset( clipped_offset );
    }

    updateContent() {
        Promise.all( this.value.commands.map( ( command ) => {
            if( command.func === 'image' )
            {
                return createImageLayerPromise( command );
            }
            else if( command.func === 'rect' )
            {
                return createRectLayerPromise( command );
            }
            else if( command.func === 'circle' )
            {
                return createCircleLayerPromise( command );
            }
            else if( command.func === 'text' )
            {
                return createTextLayerPromise( command );
            }
            else if( command.func === 'line' )
            {
                return createLineLayerPromise( command );
            }

            throw new Error( "Unknow canvas command." );
        })).then( ( layers ) => {
            layers.forEach( ( layer ) => { this.stage.add( layer ); });
        });
    }

    render() {
        return ( <div id="stage_container" ref={(c) => this.stage_container = c} style={styles.canvas}></div> );
    }
}

const styles = {
    canvas: {
        width: '100%',
        height: '100%',
        display: 'flex',
        justifyContent: 'center'
    }
};

export default CanvasElement;
