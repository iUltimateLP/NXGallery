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
    BottomNavigation,
    Backdrop,
    CircularProgress,
    CardMedia,
    Dialog,
    DialogTitle,
    DialogContent,
    DialogContentText,
    DialogActions,
    useMediaQuery,
    TableContainer,
    Table,
    TableBody,
    TableRow,
    TableCell
} = MaterialUI;

// Create a light and dark material UI theme
const lightTheme = createMuiTheme({
    palette: {
        type: "light",
        primary: {
            main: "#04c7c3",
        },
        secondary: {
            main: "#19857b",
        },
        error: {
            main: colors.red.A400,
        },
        background: {
            default: "#ebebeb",
        },
    }
});

const darkTheme = createMuiTheme({
    palette: {
        type: "dark",
        primary: {
            main: "#50f4dc",
        },
        secondary: {
            main: "#19857b",
        },
        error: {
            main: colors.red.A400,
        },
        background: {
            default: "#2d2d2d",
        }
    }
});

var isSM = window.matchMedia(lightTheme.breakpoints.down('sm').replace("@media ", "")).matches;

// Component for one gallery content
class GalleryItem extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            item: props.item,
            isVideo: props.item.type == "video",
            dialogOpen: false
        };

        this.imageClicked = this.imageClicked.bind(this);
        this.dialogClose = this.dialogClose.bind(this);
    }

    imageClicked() {
        this.setState({
            dialogOpen: true
        });
    }

    dialogClose() {
        this.setState({
            dialogOpen: false
        });
    }

    getDateString() {
        var date = new Date(this.state.item.takenAt * 1000);
        var string = date.toLocaleDateString() + " " + date.toLocaleTimeString();

        return string;
    }

    getDownloadFilename() {
        var fileName;
        var date = this.getDateString();
        date = date.replace(/[:.\s]/g, "_");
        var extension = this.state.item.type == "video" ? ".mp4" : ".jpg";

        fileName = this.state.item.game.replace(/\s/g, "_") + "_" + date + extension;
        return fileName;
    }

    render() {
        // Decide whether to show a video or an image element
        let viewElement;
        let viewElementBig;
        if (this.state.isVideo) {
            viewElement = <video src={this.state.item.path} controls preload={"none"}></video>
            viewElementBig = <video src={this.state.item.path} controls preload={"none"} className={"gallery-content-big"}></video>;
        } else {
            viewElement = <img src={this.state.item.path}></img>
            viewElementBig = <a href={this.state.item.path} target="_blank"><img src={this.state.item.path} className={"gallery-content-big"}></img></a>;
        }

        return (
            <Grid item xs={isSM ? 12 : 4}>
                <Paper onClick={this.imageClicked} elevation={2} className={"grid-cell"}>
                    {viewElement}
                </Paper>

                <Dialog fullScreen={isSM} open={this.state.dialogOpen} onClose={this.dialogClose} aria-labelledby="responsive-dialog-title">
                    <DialogContent>
                        {viewElementBig}
                        <div className={"gallery-content-bar"}>
                            <a download={this.getDownloadFilename()} href={this.state.item.path}><Button color="primary"><i className={"fas fa-download"}></i> Download</Button></a>
                        </div>
                        <TableContainer>
                            <Table>
                                <TableBody>
                                    <TableRow>
                                        <TableCell><b>Type</b></TableCell>
                                        <TableCell align="right">{this.state.item.type == "video" ? "Video" : "Screenshot"}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Taken at</b></TableCell>
                                        <TableCell align="right">{this.getDateString()}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Game</b></TableCell>
                                        <TableCell align="right">{this.state.item.game}</TableCell>
                                    </TableRow>
                                    <TableRow>
                                        <TableCell><b>Stored at</b></TableCell>
                                        <TableCell align="right">{this.state.item.storedAt == "sd" ? "SD Card" : "Internal Storage"}</TableCell>
                                    </TableRow>
                                </TableBody>
                            </Table>
                        </TableContainer>
                    </DialogContent>
                    <DialogActions>
                        <Button autoFocus color="primary" onClick={this.dialogClose}>Close</Button>
                    </DialogActions>
                </Dialog>
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
            maxPages: 1,
            currentTheme: "light",
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
                galleryContent: result.gallery,
                maxPages: result.pages,
                currentTheme: result.theme,
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
        }, () => {
            this.fetchGallery();
        });
    }

    nextPage(e) {
        // Go one page forth
        this.setState({
            currentPage: this.state.currentPage + 1
        }, () => {
            this.fetchGallery();
        });
    }

    render() {
        return (
            <ThemeProvider theme={this.state.currentTheme == "light" ? lightTheme : darkTheme}>
                <CssBaseline/>
                <Container style={{flexGrow: 1, padding: "8px"}}>
                    <Typography variant="h2" color="textPrimary" align="center">NXGallery<a href={"https://github.com/iUltimateLP/NXGallery"} target={"_blank"}><i className={`fab fa-github ${this.state.currentTheme}`}></i></a></Typography>
                    <Typography variant="h6" color="textSecondary" align="center" style={{paddingBottom: "16px"}}>Browse your Nintendo Switch album with ease!</Typography>

                    <Grid container spacing={2} justify="center">
                        {this.state.galleryContent.map((value) => (
                            <GalleryItem key={value.takenAt} item={value}/>
                        ))}
                    </Grid>

                    {this.state.error && 
                        <Typography variant="h6" color="error" align="center">Oh no, an error has occured :(</Typography>
                    }

                    {(this.state.galleryContent.length == 0 && !this.state.error) &&
                        <CircularProgress color="primary"></CircularProgress>
                    }
                    
                    {!this.state.error && <Container align="center" style={{paddingTop: "20px", paddingBottom: "12px"}}>
                        <ButtonGroup color="primary">
                            <Button onClick={() => this.prevPage()} disabled={this.state.currentPage == 1}><Icon>keyboard_arrow_left</Icon></Button>
                            <Button>{this.state.currentPage}</Button>
                            <Button onClick={() => this.nextPage()} disabled={this.state.currentPage == this.state.maxPages}><Icon>keyboard_arrow_right</Icon></Button>
                        </ButtonGroup>
                    </Container>
                    }

                    <Container align="center" className={"footer"}>
                        <Typography variant="overline" color="textSecondary" align="center">Made with</Typography>
                        <Icon className={"heart"}>favorite</Icon>
                        <Typography variant="overline" color="textSecondary" align="center">in Bremen, Germany</Typography><br/>
                    </Container>
                </Container>
            </ThemeProvider>
        );
    }
}

ReactDOM.render(
    // Render the app
    <App/>,
    document.querySelector("#app-root")
);
