import React from 'react';
import ElementComponent from './ElementComponent';
import d3 from 'd3'
import c3 from 'c3'

class ChartElement extends ElementComponent {
    static get TYPE() {
        return 2;
    }

    constructor( props, context ) {
        super( props, context );
        this.chart_id = Math.random().toString(36).replace(/[^a-z]+/g,'');
    }

    componentDidMount() {
        this.c3 = this.generateChart();
    }

    componentDidUpdate( props ) {
        this.updateChart();
    }

    CreateChartData( value, param ) {
        let chart_data = {
            json: value,
            type : param.type
        };

        if( !!param.value_map )
        {
            chart_data.xs = param.value_map;
        }

        return chart_data;
    }

    updateChart() {
        this.c3.load({
            json: this.value
        });
    }

    generateChart() {
        return c3.generate({
            bindto: '#' + this.chart_id,
            data: this.CreateChartData( this.value, this.param )
        });
    }
    render() {
         return ( <div id={this.chart_id} style={styles.chart} /> );
    }
}

const styles = {
    chart : {
        margin: '10px',
        width: '100%',
        maxWidth: '100%'
    },
};

export default ChartElement;
