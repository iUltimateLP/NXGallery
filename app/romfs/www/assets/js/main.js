/*
    NXGallery for Nintendo Switch
    Made with love by Jonathan Verbeek (jverbeek.de)
*/

// Material UI imports
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
    Modal,
    ButtonGroup,
    Icon,
    BottomNavigation
} = MaterialUI;

// Create a material UI theme
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

// Component for one gallery content
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
        // Decide whether to show a video or an image element
        let viewElement;
        if (this.state.isVideo) {
            viewElement = <video src={this.state.item.path} controls preload={"none"}></video>
        } else {
            viewElement = <img src={this.state.item.path}></img>
        }

        return (
            <Grid item xs={4}>
                <Paper onClick={this.imageClicked} elevation={2} className={"grid-cell"}>
                    {viewElement}
                </Paper>           
            </Grid>
        );
    }
}

// Component for the whole web app
class App extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            galleryContent: [],
            currentPage: 1,
            error: false
        };
    }

    componentDidMount() {
        // Fetch the gallery directly at the beginning
        this.fetchGallery();
    }

    fetchGallery() {
        fetch("/gallery?page=" + this.state.currentPage)
        .then(res => res.json())
        .then((result) => {
            this.setState({
                galleryContent: result,
                error: false
            })
        }, (error) => {
            this.setState({
                error: true
            });
        });
    }

    prevPage(e) {
        // Go one page back
        this.setState({
            currentPage: this.state.currentPage - 1
        });
    }

    nextPage(e) {
        // Go one page forth
        this.setState({
            currentPage: this.state.currentPage + 1
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
                    <Typography variant="h6" color="error" align="center">Oh no, an error has occured :(</Typography>
                }
                
                {!this.state.error && <Container align="center" style={{paddingTop: "8px"}}>
                    <ButtonGroup color="secondary">
                        <Button onClick={(e) => this.prevPage(e)} disabled={this.state.currentPage == 1}><Icon>keyboard_arrow_left</Icon></Button>
                        <Button disabled>{this.state.currentPage}</Button>
                        <Button onClick={(e) => this.nextPage(e)} disabled={this.state.currentPage == 7}><Icon>keyboard_arrow_right</Icon></Button>
                    </ButtonGroup>
                </Container>
                }

                <Container align="center" className={"footer"}>
                    <Typography variant="overline" color="textSecondary" align="center">Made with</Typography>
                    <Icon className={"heart"}>favorite</Icon>
                    <Typography variant="overline" color="textSecondary" align="center">in Bremen, Germany</Typography>
                </Container>
            </Container>
        );
    }
}

ReactDOM.render(
    // Render the app
    <ThemeProvider theme={theme}>
        <CssBaseline />
        <App />
    </ThemeProvider>,
    document.querySelector("#app-root")
);
