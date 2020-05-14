/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

const {
    colors,
    CssBaseline,
    ThemeProvider,
    Typography,
    Container,
    makeStyles,
    createMuiTheme,
    Box,
    SvgIcon,
    Link,
    Button,
    Grid,
    Paper,
    GridList,
    GridListTile,
    Modal
} = MaterialUI;

const theme = createMuiTheme({
    palette: {
      primary: {
        main: '#50f4dc', // 04c7c3
      },
      secondary: {
        main: '#19857b',
      },
      error: {
        main: colors.red.A400,
      },
      background: {
        default: '#ebebeb',
      },
    },
});

class GalleryItem extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            item: props.item,
            isVideo: props.item.path.endsWith(".mp4")
        };
    }

    imageClicked() {

    }

    render() {
        let viewElement;
        if (this.state.isVideo) {
            viewElement = <video src={this.state.item.path} controls preload={"none"}></video>
        } else {
            viewElement = <img src={this.state.item.path}></img>
        }

        return (
            <Grid item xs={4}>
                <Paper onClick={this.imageClicked()} elevation={2} className={"grid-cell"}>
                    {viewElement}
                </Paper>           
            </Grid>
        );
    }
}

class App extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            galleryContent: [],
            error: false
        };
    }

    componentDidMount() {
        fetch("/gallery?page=1")
            .then(res => res.json())
            .then((result) => {
                this.setState({
                    galleryContent: result,
                    error: false
                })
            }, (error) => {
                console.log("Error");
                this.setState({
                    error: true
                });
            });
    }

    render() {
        return (
            <Container style={{flexGrow: 1, padding: "8px"}}>
                <Typography variant="h2" color="textPrimary" align="center">NXGallery</Typography>
                <Typography variant="h6" color="textSecondary" align="center" style={{paddingBottom: "16px"}}>Browse your Nintendo Switch album with ease!</Typography>

                <Grid container spacing={2} justify="center">
                    {this.state.galleryContent.map((value) => (
                        <GalleryItem key={value.takenAt} item={value} />
                    ))}
                </Grid>

                {this.state.error && 
                    <Typography variant="h6" color="error" align="center">An error has occured.</Typography>
                }
                
            </Container>
        );
    }
}

ReactDOM.render(
    <ThemeProvider theme={theme}>
        <CssBaseline />
        <App />
    </ThemeProvider>,
    document.querySelector("#app-root")
);
