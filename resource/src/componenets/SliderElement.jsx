import React from 'react';
import ElementComponent from './ElementComponent';
import Slider from 'material-ui/Slider';

class SliderElement extends ElementComponent {
    static get TYPE() {
        return 5;
    }

    constructor( props, context ) {
        super( props, context );
        this.ws = props.ws;
        this.setValue = ( value ) => props.setValue( this.ws, { id: this.id, type: SliderElement.TYPE, value } );
    }

    componentWillUpdate( nextProps ) {
        super.componentWillUpdate( nextProps );
        this.ws = nextProps.ws;
    }

    updateValue( value ) {
        this.value = value;
        this.setValue( this.value );
    }

    render() {
        return (
            <Slider
                style={styles.slider}
                sliderStyle={styles.slider2}
                description={this.param.label}
                onChange={ ( event,value ) => this.updateValue( value ) }
                value={this.value} />
        );
    }
}

const styles = {
    slider: {
        flex: '1',
        margin: '8px',
    },
    slider2: {
        margin: '0px'
    },
};

export default SliderElement;
