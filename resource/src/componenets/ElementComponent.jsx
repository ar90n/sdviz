import React from 'react';

class ElementComponent extends React.Component {

    static get propTypes() {
        return {
            style: React.PropTypes.object.isRequired,
            id: React.PropTypes.string.isRequired,
            value: React.PropTypes.object.isRequired,
            param: React.PropTypes.object.isRequired,
            version: React.PropTypes.number.isRequired,
        }
    }

    storeProps( props ) {
        this.style = props.style;
        this.id = props.id;
        this.value = props.value;
        this.param = props.param;
        this.version = props.version;
    }

    constructor( props, context ) {
        super( props, context );
        this.storeProps( props );
    }

    shouldComponentUpdate( nextProps ) {
        return nextProps.version !== this.version;
    }

    componentWillUpdate( nextProps ) {
        this.storeProps( nextProps );
    }
};

export default ElementComponent;
