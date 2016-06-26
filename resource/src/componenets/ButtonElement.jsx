import React from 'react';
import ElementComponent from './ElementComponent';
import FlatButton from 'material-ui/FlatButton';

class ButtonElement extends ElementComponent {
    static get TYPE() {
        return 4;
    }

    constructor( props, context ) {
        super( props, context );
        this.ws = props.ws;
        this.setValue = ( value ) => props.setValue( this.ws, { id: this.id, type: ButtonElement.TYPE, value } );
    }

    componentWillUpdate( nextProps ) {
        super.componentWillUpdate( nextProps );
        this.ws = nextProps.ws;
    }

    render() {
        return (
            <FlatButton
                label={this.param.label}
                style={styles.button}
                onClick={() => this.setValue( false )}
                fullWidth={true} />
        );
    }
}

const styles = {
    button: {
        flex: '1'
    }
};

export default ButtonElement;
