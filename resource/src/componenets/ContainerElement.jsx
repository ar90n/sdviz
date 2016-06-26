import React from 'react';
import { Card, CardHeader } from 'material-ui/Card';

import ElementComponent from './ElementComponent';
import TextElement from './TextElement';
import CanvasElement from './CanvasElement';
import ChartElement from './ChartElement';
import ButtonElement from './ButtonElement';
import SliderElement from './SliderElement';

class ContainerElement extends ElementComponent {
    static get TYPE() {
        return 3;
    }

    constructor( props, context ) {
        super( props, context );
        this.elements = props.elements;
        this.setValue = props.setValue;
        this.ws = props.ws;
        this.with_card = props.with_card;
    }

    componentWillUpdate( nextProps ) {
        super.componentWillUpdate( nextProps );
        this.elements = nextProps.elements;
        this.setValue = nextProps.setValue;
        this.ws = nextProps.ws;
        this.with_card = nextProps.with_card;
    }

    shouldComponentUpdate( nextProps ) {
        return true;
    }

    createComponents() {
        const CreateElement = ( style, id ) => {
            const common_props = Object.assign( {}, this.elements[id], { id, style } );
            const sync_props = { setValue: this.setValue, ws: this.ws };
            const container_props = { elements: this.elements, with_card: true };
            switch (common_props.type) {
                case TextElement.TYPE: return <TextElement {...common_props} />;
                case CanvasElement.TYPE: return <CanvasElement {...common_props}  />;
                case ChartElement.TYPE: return <ChartElement {...common_props} />;
                case ContainerElement.TYPE: return <ContainerElement {...common_props} {...sync_props} {...container_props} />;
                case ButtonElement.TYPE: return <ButtonElement {...common_props} {...sync_props} />;
                case SliderElement.TYPE: return <SliderElement {...common_props} {...sync_props} />;
                default: return <div></div>;
            }
        };

        return this.elements[ this.id ].value
                .reduce( ( previous, { id, span } ) => {
                    if( span < 0 )
                    {
                        previous.push( {row_size:0, row_layout:[]} );
                    }
                    else
                    {
                        previous[ previous.length - 1 ].row_layout.push( { id, span } );
                        previous[ previous.length - 1 ].row_size += span;
                    }
                    return previous;
                }, [{row_size:0, row_layout:[]}] )
                .map( ( { row_size, row_layout } ) => {
                    const row_components = row_layout.map( ( { id, span } ) => {
                        const width_percentage = (100.0 * span) / row_size;
                        const width_str =`${width_percentage}%`;
                        const cur_element_style = Object.assign( {}, styles.element, { width: width_str } );
                        return CreateElement( cur_element_style, id );
                    });

                    const cur_flex_direction = this.param.is_row_direction ? 'row' : 'column';
                    const cur_container_style = Object.assign( {}, styles.container, { flexDirection: cur_flex_direction } );
                    return <div style={cur_container_style}> { row_components } </div>;
                });
    }

    render() {
        const children_components = this.createComponents();
        if( this.with_card )
        {
            return (
                <Card style={this.style} containerStyle={styles.cardContainer} >
                    {(() => { if( !!this.param.label ) { return <CardHeader titleStyle={ styles.title } title={ this.param.label } /> } })()}
                    { children_components }
                </Card>
            );
        }
        else
        {
            return (
                <div>
                    { children_components }
                </div>
            );
        }
    }
};

const styles = {
    container:{
        display: 'flex',
        flexDirection: 'row',
        justifyContent: 'center'
    },
    cardContainer:{
        flex: '1',
        width: '100%',
    },
    element:{
        display: 'flex',
        flex: '1',
        margin: '4px',
        alignItems: 'center'
    },
    title: {
        fontWeight: 'bold'
    }
};
export default ContainerElement;
