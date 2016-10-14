import React from 'react';
import {Buffer} from 'buffer';
import LZ4 from 'lz4';
import Konva  from 'konva';
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

function createImageLayerPromise( command )
{
    const src_image = command.args[0];
    const left_position = command.args[1];
    const top_position = command.args[2];
    const opacity = 255 * command.args[3];

    const image_size = getImageSize( src_image );
    src_image.buffer = uncompressLZ4( src_image.buffer, image_size );

    const layer = new Konva.Layer();
    const ctx = layer.getContext();
    const dst_image = ctx.createImageData( src_image.width, src_image.height );
    for( let i = 0; i < src_image.width * src_image.height; i++ )
    {
        dst_image.data[ 4 * i + 0 ] = src_image.buffer[ 3 * i + 0 ];
        dst_image.data[ 4 * i + 1 ] = src_image.buffer[ 3 * i + 1 ];
        dst_image.data[ 4 * i + 2 ] = src_image.buffer[ 3 * i + 2 ];
        dst_image.data[ 4 * i + 3 ] = opacity;
    }

    return createImageBitmap( dst_image ).then( ( bitmap ) => {
        layer.add( new Konva.Image({ image: bitmap }) );
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

function clipScale( scale, value_width, value_height, stage_width, stage_height, max_scale )
{
    const min_scale_x = Math.min( 1.0, stage_width / value_width );
    const min_scale_y = Math.min( 1.0, stage_height / value_height );
    const min_scale = Math.min( min_scale_x, min_scale_y );
    return Math.max( min_scale, Math.min( scale, max_scale ) );
}

const max_scale = 15.0;
const unit_scale_dist = 120;

class CanvasElement extends ElementComponent {
    static get TYPE() { return 1; }

    constructor( props, context ) {
        super( props, context );

        this.resizeListener = ( e ) => {
            this.updatePosition();
            this.stage.draw();
        };
        this.mousedownListener = ( e ) => {
            if(this.stage) {
                this.modifier_key_status = getModifierKeyStatus( e.evt.shiftKey, e.evt.ctrlKey );
                this.press_x = e.evt.pageX;
                this.press_y = e.evt.pageY;
                this.press_stage_offset_x = this.stage.offsetX();
                this.press_stage_offset_y = this.stage.offsetY();
                this.press_stage_scale = this.stage.scaleX();
            }
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

            const diff_x = e.pageX - this.press_x;
            const diff_y = e.pageY - this.press_y;
            if( this.modifier_key_status == 0 ) {
                const next_x = (-diff_x / this.stage.scaleX()) + this.press_stage_offset_x;
                const next_y = (-diff_y / this.stage.scaleY()) + this.press_stage_offset_y;
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
                const next_scale = ( -diff_y / unit_scale_dist ) + this.press_stage_scale;
                const next_clipped_scale = clipScale( next_scale,
                                                      this.value.width,
                                                      this.value.height,
                                                      this.stage.width(),
                                                      this.stage.height(),
                                                      max_scale );
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
                                              max_scale );
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
