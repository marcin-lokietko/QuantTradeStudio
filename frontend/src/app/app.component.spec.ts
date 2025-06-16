import { ComponentFixture, TestBed } from '@angular/core/testing';
import { AppComponent } from './app.component';
import { Router } from '@angular/router';

describe('AppComponent', () => {
  let fixture: ComponentFixture<AppComponent>;
  let app: AppComponent;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [AppComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(AppComponent);
    app = fixture.componentInstance;
  });

  it('should create the component', () => {
    expect(app).toBeTruthy();
  });

  it(`should have the 'algo-trader-gui' title`, () => {
    expect(app.title).toEqual('algo-trader-gui');
  });

  it(`menu should allow to navigate to: assets, orders, bots`, () => {
    const router = TestBed.inject(Router);

    const navigateSpy = spyOn(router, 'navigate');

    app.navigateToAssets();
    expect(navigateSpy).toHaveBeenCalledWith(['/account/assets']);

    app.navigateToOrders();
    expect(navigateSpy).toHaveBeenCalledWith(['/account/orders']);

    app.navigateToBots();
    expect(navigateSpy).toHaveBeenCalledWith(['/bots/launch']);
  });
});
